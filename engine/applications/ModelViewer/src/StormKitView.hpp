#pragma once

#include <QtQuick/QQuickItem>

#include <stormkit/engine/Engine.hpp>
#include <stormkit/engine/Fwd.hpp>

class StormKitNode;
class StormKitView: public QQuickItem {
    Q_OBJECT
    QML_NAMED_ELEMENT(StormKitView)

  public:
    explicit StormKitView(QQuickItem *parent = nullptr);
    ~StormKitView() override;

  protected:
    auto updatePaintNode(QSGNode *, UpdatePaintNodeData *) -> QSGNode * override;
    auto geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) -> void override;

  private Q_SLOTS:
    auto invalidateSceneGraph() -> void;

  private:
    auto releaseResources() -> void override;

    StormKitNode *m_node = nullptr;

    stormkit::engine::EngineOwnedPtr m_engine;
};
