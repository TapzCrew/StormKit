// Copyright (C) 2023 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <llfio/llfio.hpp>

module IFC;

import std;

import stormkit.Core;

import :Parser;
import :Partition;

using namespace stormkit;

namespace llfio = LLFIO_V2_NAMESPACE;

struct Parser::ParseState {
    llfio::mapped_file_handle file;
    IFCFile output;

    Header header = {};
    std::vector<PartitionEntry> partition_entries;
    std::vector<std::string> string_table;
    ScopeDescriptor global_scope;
};

/////////////////////////////////////
/////////////////////////////////////
Parser::Parser() = default;

/////////////////////////////////////
/////////////////////////////////////
auto Parser::load(const std::filesystem::path& path) noexcept -> Expected<IFCFile> {
    auto parser = Parser {};

    auto result = parser.loadImpl(path);
    if (!result) return std::unexpected(result.error());

    return { *std::move(result) };
}

/////////////////////////////////////
/////////////////////////////////////
auto Parser::loadImpl(const std::filesystem::path& path) noexcept -> Expected<IFCFile> {
    auto state = ParseState {};
    {
        auto ret = llfio::mapped_file({}, path);
        if (!ret) return std::unexpected(Error::FailedToOpenFile);
        state.file = std::move(ret).value();
    }

    {
        auto ret = doParseHeader(std::move(state));
        if (!ret) return std::unexpected(ret.error());
        state = *std::move(ret);
    }

    {
        auto ret = doParseStringTable(std::move(state));
        if (!ret) return std::unexpected(ret.error());
        state = *std::move(ret);
    }

    {
        auto ret = doParsePartitions(std::move(state));
        if (!ret) return std::unexpected(ret.error());
        state = *std::move(ret);
    }

    {
        auto ret = doParseGlobalScope(std::move(state));
        if (!ret) return std::unexpected(ret.error());
        state = *std::move(ret);
    }

    state.output.src_path      = state.string_table[state.header.src_path];
    state.output.major_version = core::as<core::UInt32>(state.header.major_version);
    state.output.minor_version = core::as<core::UInt32>(state.header.minor_version);
    state.output.abi           = core::as<core::UInt32>(state.header.abi);
    state.output.arch          = state.header.arch;
    state.output.dialect       = state.header.dialect;
    state.output.unit          = state.header.unit;

    return { std::move(state.output) };
}

/////////////////////////////////////
/////////////////////////////////////
auto Parser::doParseHeader(ParseState&& state) noexcept -> Expected<ParseState> {
    constexpr auto HEADER_SIZE = sizeof(Header);
    using RawHeader            = const core::ByteStaticArray<HEADER_SIZE>;

    auto& header = state.header;

    const auto *start_addr = state.file.address();

    const auto *header_addr   = reinterpret_cast<const RawHeader *>(start_addr);
    const auto *header2       = reinterpret_cast<const Header *>(start_addr);
    header                    = std::bit_cast<Header>(*header_addr);
    header.dialect            = core::toEndian(header.dialect, std::endian::little);
    header.string_table_bytes = core::toEndian(header.string_table_bytes, std::endian::little);
    header.string_table_size  = core::toEndian(header.string_table_size, std::endian::little);
    header.unit               = std::bit_cast<UnitIndex>(
        core::toEndian(std::bit_cast<core::UInt32>(header.unit), std::endian::little));
    header.src_path        = core::toEndian(header.src_path, std::endian::little);
    header.global_scope    = core::toEndian(header.global_scope, std::endian::little);
    header.toc             = core::toEndian(header.toc, std::endian::little);
    header.partition_count = core::toEndian(header.partition_count, std::endian::little);

    if (header.signature != core::toEndian(IFC_SIGNATURE, std::endian::little))
        return std::unexpected(Error::SignatureMismatch);

    return state;
}

/////////////////////////////////////
/////////////////////////////////////
auto Parser::doParseStringTable(ParseState&& state) noexcept -> Expected<ParseState> {
    auto& string_table = state.string_table;

    const auto *start_addr        = state.file.address();
    const auto *string_table_addr = start_addr + state.header.string_table_bytes;
    const auto *string_table_end  = string_table_addr + state.header.string_table_size;

    for (auto *str_addr = string_table_addr + 1; str_addr != string_table_end;) {
        const auto *raw_str = reinterpret_cast<const char *>(str_addr);
        auto& str = string_table.emplace_back(raw_str, std::char_traits<char>::length(raw_str));
        core::println("Parsing str at {:#x} of size {}",
                      std::bit_cast<std::uintptr_t>(str_addr),
                      std::size(str));
        str_addr += (std::size(str) + 1) * sizeof(char);
        // str_addr += (std::size(str) + 1) * sizeof(char);
    }

    return state;
}

/////////////////////////////////////
/////////////////////////////////////
auto Parser::doParsePartitions(ParseState&& state) noexcept -> Expected<ParseState> {
    constexpr auto HEADER_SIZE = sizeof(Header);
    constexpr auto ENTRY_SIZE  = sizeof(PartitionEntry);
    using RawPartitionEntry    = const core::ByteStaticArray<ENTRY_SIZE>;

    auto& partition_entries = state.partition_entries;

    const auto *start_addr            = state.file.address();
    const auto *partitions_addr       = start_addr + HEADER_SIZE;
    const auto *table_of_content_addr = partitions_addr + state.header.toc;

    partition_entries.reserve(state.header.partition_count);
    state.output.partitions.reserve(state.header.partition_count);
    for (auto i : core::range(state.header.partition_count)) {
        const auto raw_entry =
            reinterpret_cast<const RawPartitionEntry *>(partitions_addr + i * ENTRY_SIZE);
        partition_entries.emplace_back(std::bit_cast<PartitionEntry>(*raw_entry));
    }

    for (const auto& entry : partition_entries) {
        if (entry.name >= std::size(state.string_table)) state.output.partitions.emplace_back("");
        else
            state.output.partitions.emplace_back(
                IFCFile::Partition { state.string_table[entry.name] });
    }

    return state;
}

/////////////////////////////////////
/////////////////////////////////////
auto Parser::doParseGlobalScope(ParseState&& state) noexcept -> Expected<ParseState> {
    constexpr auto GLOBAL_SCOPE_SIZE = sizeof(ScopeDescriptor);
    using RawScopeDescriptor         = const core::ByteStaticArray<GLOBAL_SCOPE_SIZE>;

    auto& scope = state.global_scope;

    const auto *start_addr = state.file.address();

    const auto *raw_global_scope =
        reinterpret_cast<const RawScopeDescriptor *>(start_addr + state.header.global_scope);
    scope = std::bit_cast<ScopeDescriptor>(*raw_global_scope);

    return state;
}
