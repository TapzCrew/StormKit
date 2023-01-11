module;

#include <QMap>
#include <QObjectData>

module StormKitNode;

////////////////////////////////////////
////////////////////////////////////////
StormKitNode::StormKitNode(QQuickItem *item) : m_item { item } {
}

////////////////////////////////////////
////////////////////////////////////////
StormKitNode::~StormKitNode() = default;

////////////////////////////////////////
////////////////////////////////////////
auto StormKitNode::texture() const -> QSGTexture * {
    return nullptr;
}

////////////////////////////////////////
////////////////////////////////////////
auto StormKitNode::sync() -> void {
}

////////////////////////////////////////
////////////////////////////////////////
auto StormKitNode::render() -> void {
}

////////////////////////////////////////
////////////////////////////////////////
auto StormKitNode::initialize() -> void {
}
