// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>
#include <QtQuickControls2/QQuickStyle>

#include "StormKitView.hpp"

auto main(int argc, char *argv[]) -> int {
    auto app = QGuiApplication { argc, argv };

    auto engine = QQmlApplicationEngine {};
    engine.addImportPath("qrc:///");

    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);

#ifdef Q_OS_WINDOWS
    app.setWindowIcon(QIcon { QStringLiteral("qrc:/assets/glTF.ico") });

    QQuickWindow::setDefaultAlphaBuffer(true);
    QQuickStyle::setStyle("WinUI3Style");
    const auto url = QUrl { QStringLiteral("qrc:///MainWindows.qml") };
#elif Q_OS_MACOS
    const auto url = QUrl { QStringLiteral("qrc:///MainMacOS.qml") };

#elif defined(Q_OS_LINUX)
    const auto url = QUrl { QStringLiteral("qrc:///MainLinux.qml") };

#endif

    qmlRegisterType<StormKitView>("ModelViewer", 1, 0, "StormKitView");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
