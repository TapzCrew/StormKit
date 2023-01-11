module;

#include <QMap>
#include <QObjectData>

module StormKitView;

import stormkit.core.Math;

using namespace stormkit;

////////////////////////////////////////
////////////////////////////////////////
StormKitView::StormKitView(QQuickItem *parent) : QQuickItem { parent } {
    setFlag(ItemHasContents, true);
}

////////////////////////////////////////
////////////////////////////////////////
StormKitView::~StormKitView() = default;

////////////////////////////////////////
////////////////////////////////////////
auto StormKitView::updatePaintNode(QSGNode *, UpdatePaintNodeData *) -> QSGNode * {
    if (!m_engine && width() != 0 && height() != 0) {
        m_engine = std::make_unique<engine::Engine>(
            core::math::ExtentU { core::as<core::UInt32>(width()),
                                  core::as<core::UInt32>(height()) });
    }
    return nullptr;
}

////////////////////////////////////////
////////////////////////////////////////
/*auto StormKitView::geometryChange([[maybe_unused]] const QRectF& newGeometry,
                                  [[maybe_unused]] const QRectF& oldGeometry) -> void {
}*/

////////////////////////////////////////
////////////////////////////////////////
auto StormKitView::releaseResources() -> void {
    m_node = nullptr;
}

////////////////////////////////////////
////////////////////////////////////////
auto StormKitView::invalidateSceneGraph() -> void {
    m_node = nullptr;
}
