/////////// - StormKit::image - ///////////
#include <storm/core/Types.hpp>

/////////// - StormKit::image - ///////////
#include <storm/image/Image.hpp>

/////////// - StormKit::engine - ///////////
#include <storm/engine/render/3D/Model.hpp>
#include <storm/engine/render/3D/PbrMaterial.hpp>

#include "../../Log.hpp"

/////////// - TinyGLTF - ///////////
#define TINYGLTF_USE_CPP14
#define TINYGLTF_NOEXCEPTION
#define TINYGLTF_USE_RAPIDJSON
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_RAPIDJSON
#include "../../data/tiny_gltf.hpp"

using namespace storm;
using namespace storm::engine;

STORMKIT_PRIVATE bool LoadImageData(tinygltf::Image *image,
                                    [[maybe_unused]] const int image_idx,
                                    [[maybe_unused]] std::string *err,
                                    [[maybe_unused]] std::string *warn,
                                    [[maybe_unused]] int req_width,
                                    [[maybe_unused]] int req_height,
                                    const unsigned char *bytes,
                                    int size,
                                    void *) {
    /*auto _image = image::Image { core::toConstByteSpan(bytes, size) }.toFormat(
        image::Image::Format::RGBA8_UNorm);
    // if (_image.codec() == image::Image::Codec::PNG) _image = image::Image::flipY(_image);

    int pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;

    image->width      = _image.extent().width;
    image->height     = _image.extent().height;
    image->component  = _image.channelCount();
    image->bits       = 8 * _image.bytesPerChannel();
    image->pixel_type = pixel_type;

    const auto _size = image->width * image->height * image->component * image->bits / 8u;
    image->image.resize(_size);

    const auto data = _image.data();

    core::ranges::copy(core::ranges::begin(data),
                       core::ranges::begin(data) + _size,
                       reinterpret_cast<core::Byte *>(std::data(image->image)));*/

    return true;
}

/////////////////////////////////////
/////////////////////////////////////
Model::Model(Engine &engine) noexcept : m_engine { engine } {
}

/////////////////////////////////////
/////////////////////////////////////
Model::Model(Engine &engine, std::filesystem::path path) : Model { engine } {
    load(std::move(path));
}

/////////////////////////////////////
/////////////////////////////////////
Model::~Model() = default;

/////////////////////////////////////
/////////////////////////////////////
Model::Model(Model &&) noexcept = default;

/////////////////////////////////////
/////////////////////////////////////
auto Model::operator=(Model &&) noexcept -> Model & = default;

/////////////////////////////////////
/////////////////////////////////////
void Model::load(std::filesystem::path path) {
    m_path = std::move(path);

    auto loader = tinygltf::TinyGLTF {};
    loader.SetImageLoader(::LoadImageData, nullptr);

    auto model = tinygltf::Model {};

    auto error   = std::string {};
    auto warning = std::string {};

    auto res = false;
    if (m_path.string().back() == 'b' || m_path.string().back() == 'B')
        res = loader.LoadBinaryFromFile(&model, &error, &warning, m_path.string());
    else
        res = loader.LoadASCIIFromFile(&model, &error, &warning, m_path.string());

    if (!std::empty(error)) elog("Failed to open {}, reason: ({})", m_path.string(), error);
    if (!std::empty(warning)) wlog("TinyGLTF warning, ({})", warning);

    if (!res) return;

    auto scene = model.scenes[model.defaultScene];

    const auto name = scene.name;

    auto output_mesh = PbrMesh { name };

    for (const auto &skin : model.skins) {
        auto mesh_skin = doParseSkin(model, skin);

        output_mesh.addSkin(std::move(mesh_skin));
    }

    for (const auto &animation : model.animations) {
        auto mesh_animation = doParseAnimation(model, animation);

        output_mesh.addAnimation(std::move(mesh_animation));
    }

    auto parents = core::HashMap<PbrMesh::Node::ID, PbrMesh::Node::ID> {};

    auto nodes = std::deque<PbrMesh::Node::ID> {};
    for (const auto node_id : scene.nodes) {
        nodes.emplace_back(node_id);

        parents[node_id] = PbrMesh::Node::INVALID;
    }

    while (!std::empty(nodes)) {
        const auto node_id = nodes.front();
        nodes.pop_front();

        const auto &node = model.nodes[node_id];

        for (const auto child_id : node.children) {
            parents[child_id] = node_id;

            nodes.emplace_back(child_id);
        }
    }

    auto i = 0;
    for (const auto &node : model.nodes) {
        auto mesh_node   = PbrMesh::Node {};
        mesh_node.parent = parents[i++];

        if (!std::empty(node.translation))
            mesh_node.translation = core::make_vec3(std::data(node.translation));
        if (!std::empty(node.rotation))
            mesh_node.rotation = core::Quaternion { core::make_quat(std::data(node.rotation)) };
        if (!std::empty(node.scale)) mesh_node.scale = core::make_vec3(std::data(node.scale));
        if (!std::empty(node.matrix)) mesh_node.matrix = core::make_mat4(std::data(node.matrix));

        if (node.mesh >= 0) {
            const auto &mesh = model.meshes[node.mesh];

            auto subdrawable = doParseMesh(model, mesh);
            auto id          = output_mesh.addSubdrawable(std::move(subdrawable));

            mesh_node.sub_drawable_id = id;
        }

        if (node.skin >= 0) mesh_node.skin = node.skin;

        for (const auto child_id : node.children) mesh_node.childs.emplace_back(child_id);

        output_mesh.addNode(std::move(mesh_node));
    }

    m_mesh = std::move(output_mesh);
}

/////////////////////////////////////
/////////////////////////////////////
auto Model::createMesh() const noexcept -> PbrMeshOwnedPtr {
    return PbrMesh::allocateOwned(m_mesh);
}

/////////////////////////////////////
/////////////////////////////////////
auto Model::createMaterial() const noexcept -> PbrMaterialOwnedPtr {
    return {};
}

////////////////////////////////////////
////////////////////////////////////////
SubDrawable Model::doParseMesh(const tinygltf::Model &model, const tinygltf::Mesh &mesh) {
    auto sub_drawable = SubDrawable { mesh.name };

    auto vertex_array = VertexArray {};

    auto first_index  = 0u;
    auto vertex_start = 0u;
    for (const auto &primitive : mesh.primitives) {
        auto _primitive = doParsePrimitive(model, primitive);
        for (auto &indice : _primitive.indices) indice += vertex_start;

        _primitive.first_index = first_index;
        first_index += std::size(_primitive.indices);
        vertex_start += _primitive.vertices.count<PbrMesh::Vertex>();

        sub_drawable.primitives.emplace_back(std::move(_primitive));
    }

    return sub_drawable;
}

////////////////////////////////////////
////////////////////////////////////////
DrawablePrimitive Model::doParsePrimitive(const tinygltf::Model &model,
                                          const tinygltf::Primitive &primitive) {
    auto _primitive = DrawablePrimitive {};

    auto vertex_max = core::Vector3f { 0.f, 0.f, 0.f };
    auto vertex_min = core::Vector3f { 0.f, 0.f, 0.f };

    for (const auto &attribute : primitive.attributes) {
        const auto &accessor    = model.accessors[attribute.second];
        const auto &buffer_view = model.bufferViews[accessor.bufferView];
        const auto &buffer      = model.buffers[buffer_view.buffer];
        const auto *buffer_data =
            reinterpret_cast<const std::byte *>(&buffer.data[buffer_view.byteOffset]);

        auto stride = accessor.ByteStride(buffer_view);
        if (stride < 0) {
            stride = tinygltf::GetComponentSizeInBytes(accessor.componentType) *
                     tinygltf::GetNumComponentsInType(accessor.type);
        }

        const auto count = accessor.count;

        if (count > std::size(_primitive.vertices)) {
            _primitive.vertices.resize(count * sizeof(PbrMesh::Vertex));
        }

        for (auto i = 0u; i < count; ++i) {
            const auto it = buffer_data + accessor.byteOffset + i * stride;

            auto &vertex = _primitive.vertices.at<PbrMesh::Vertex>(i);

            if (attribute.first.compare("POSITION") == 0) {
                vertex_max = core::make_vec3(std::data(accessor.maxValues));
                vertex_min = core::make_vec3(std::data(accessor.minValues));

                vertex.position = core::make_vec3(reinterpret_cast<const float *>(it));
            } else if (attribute.first.compare("NORMAL") == 0)
                vertex.normal = core::make_vec3(reinterpret_cast<const float *>(it));
            else if (attribute.first.compare("TANGENT") == 0)
                vertex.tangent = core::make_vec4(reinterpret_cast<const float *>(it));
            else if (attribute.first.compare("TEXCOORD_0") == 0) {
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    vertex.texcoord = core::make_vec2<float>(reinterpret_cast<const float *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    vertex.texcoord =
                        core::make_vec2<core::UInt8>(reinterpret_cast<const core::UInt8 *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    vertex.texcoord =
                        core::make_vec2<core::UInt16>(reinterpret_cast<const core::UInt16 *>(it));
                }
            } else if (attribute.first.compare("JOINTS_0") == 0) {
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    vertex.joint_id =
                        core::make_vec4<core::UInt8>(reinterpret_cast<const core::UInt8 *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    vertex.joint_id =
                        core::make_vec4<core::UInt16>(reinterpret_cast<const core::UInt16 *>(it));
                }
            } else if (attribute.first.compare("WEIGHTS_0") == 0) {
                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    vertex.weight = core::make_vec4<float>(reinterpret_cast<const float *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                    vertex.weight =
                        core::make_vec4<core::UInt8>(reinterpret_cast<const core::UInt8 *>(it));
                } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                    vertex.weight =
                        core::make_vec4<core::UInt16>(reinterpret_cast<const core::UInt16 *>(it));
                }
            }
        }
    }

    _primitive.bounding_box.min    = std::move(vertex_min);
    _primitive.bounding_box.max    = std::move(vertex_max);
    _primitive.bounding_box.extent = core::Extentf {
        _primitive.bounding_box.max.x - _primitive.bounding_box.min.x,
        _primitive.bounding_box.max.y - _primitive.bounding_box.min.y,
        _primitive.bounding_box.max.z - _primitive.bounding_box.min.z,
    };

    if (primitive.indices >= 0) {
        const auto &accessor    = model.accessors[primitive.indices];
        const auto &buffer_view = model.bufferViews[accessor.bufferView];
        const auto &buffer      = model.buffers[buffer_view.buffer];
        const auto *buffer_data = reinterpret_cast<const std::byte *>(
            &buffer.data[accessor.byteOffset + buffer_view.byteOffset]);

        const auto size = tinygltf::GetComponentSizeInBytes(accessor.componentType);

        const auto fill_indices = [&buffer_data, size](auto &indices) {
            if (size == 1) {
                const auto data = reinterpret_cast<const core::UInt8 *>(buffer_data);
                auto i          = core::ArraySize { 0 };
                for (auto &indice : indices) {
                    indice = data[i];
                    ++i;
                }
            } else if (size == 2) {
                const auto data = reinterpret_cast<const core::UInt16 *>(buffer_data);
                auto i          = core::ArraySize { 0 };
                for (auto &indice : indices) {
                    indice = data[i];
                    ++i;
                }
            } else if (size == 4) {
                const auto data = reinterpret_cast<const core::UInt32 *>(buffer_data);
                auto i          = core::ArraySize { 0 };
                for (auto &indice : indices) {
                    indice = data[i];
                    ++i;
                }
            }
        };

        _primitive.indices.resize(accessor.count);

        fill_indices(_primitive.indices);
    }

    return _primitive;
}

////////////////////////////////////////
////////////////////////////////////////
PbrMesh::Skin Model::doParseSkin(const tinygltf::Model &model, const tinygltf::Skin &skin) {
    auto mesh_skin = PbrMesh::Skin {};
    mesh_skin.name = skin.name;

    for (const auto joint_id : skin.joints) mesh_skin.joints.emplace_back(joint_id);

    if (skin.inverseBindMatrices >= 0) {
        const auto &accessor    = model.accessors[skin.inverseBindMatrices];
        const auto &buffer_view = model.bufferViews[accessor.bufferView];
        const auto &buffer      = model.buffers[buffer_view.buffer];

        const auto offset = accessor.byteOffset + buffer_view.byteOffset;

        mesh_skin.inverse_bind_matrices.resize(accessor.count);
        std::memcpy(std::data(mesh_skin.inverse_bind_matrices),
                    &buffer.data[offset],
                    accessor.count * sizeof(core::Matrixf));
    }

    return mesh_skin;
}

////////////////////////////////////////
////////////////////////////////////////
PbrMesh::Animation Model::doParseAnimation(const tinygltf::Model &model,
                                           const tinygltf::Animation &animation) {
    auto mesh_animation = PbrMesh::Animation {};
    mesh_animation.samplers.reserve(std::size(animation.samplers));
    mesh_animation.channels.reserve(std::size(animation.channels));
    mesh_animation.name = animation.name;

    for (auto &sampler : animation.samplers) {
        auto &mesh_sampler = mesh_animation.samplers.emplace_back();

        if (sampler.interpolation == "LINEAR")
            mesh_sampler.interpolation = PbrMesh::InterpolationType::Linear;
        else if (sampler.interpolation == "STEP")
            mesh_sampler.interpolation = PbrMesh::InterpolationType::Step;
        else if (sampler.interpolation == "CUBICSPLINE")
            mesh_sampler.interpolation = PbrMesh::InterpolationType::Cubic_Spline;

        {
            const auto &accessor    = model.accessors[sampler.input];
            const auto &buffer_view = model.bufferViews[accessor.bufferView];
            const auto &buffer      = model.buffers[buffer_view.buffer];

            const auto offset = accessor.byteOffset + buffer_view.byteOffset;

            const auto data = reinterpret_cast<const float *>(&buffer.data[offset]);

            mesh_sampler.inputs.resize(accessor.count);
            core::ranges::copy(data,
                               data + accessor.count,
                               core::ranges::begin(mesh_sampler.inputs));

            for (auto input : mesh_sampler.inputs) {
                const auto input_time = core::Secondf { input };
                if (input_time < mesh_animation.start) mesh_animation.start = input_time;
                if (input_time > mesh_animation.end) mesh_animation.end = input_time;
            }
        }

        {
            const auto &accessor    = model.accessors[sampler.output];
            const auto &buffer_view = model.bufferViews[accessor.bufferView];
            const auto &buffer      = model.buffers[buffer_view.buffer];

            const auto offset = accessor.byteOffset + buffer_view.byteOffset;

            const auto data = reinterpret_cast<const float *>(&buffer.data[offset]);

            mesh_sampler.outputs.reserve(accessor.count);
            switch (accessor.type) {
                case TINYGLTF_TYPE_VEC3:
                    for (auto i = 0u; i < accessor.count; ++i) {
                        mesh_sampler.outputs.emplace_back(
                            core::make_vec4(core::make_vec3(&data[i * 3])));
                    }
                    break;
                case TINYGLTF_TYPE_VEC4:
                    for (auto i = 0u; i < accessor.count; ++i) {
                        mesh_sampler.outputs.emplace_back(core::make_vec4(&data[i * 4]));
                    }
                    break;
            }
        }
    }

    for (const auto &channel : animation.channels) {
        auto &mesh_channel = mesh_animation.channels.emplace_back();

        mesh_channel.node = channel.target_node;
        if (channel.target_path == "translation") mesh_channel.path = PbrMesh::Path::Translation;
        else if (channel.target_path == "rotation")
            mesh_channel.path = PbrMesh::Path::Rotation;
        else if (channel.target_path == "scale")
            mesh_channel.path = PbrMesh::Path::Scale;
        else if (channel.target_path == "weight")
            mesh_channel.path = PbrMesh::Path::Weights;

        mesh_channel.sampler = channel.sampler;
    }

    return mesh_animation;
}
