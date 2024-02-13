#include "wasmmap.hpp"

#include <emscripten.h>
#include <emscripten/bind.h>


EM_JS(void, js_setVisible, (const bool visible), {
    if(visible)
    {
        document.getElementById("map").style.zIndex = "1000";
        document.getElementById("qtrootDiv").style.zIndex = "1";
    }
    else
    {
        document.getElementById("map").style.zIndex = "1";
        document.getElementById("qtrootDiv").style.zIndex = "1000";
    }
});

ObjectMapView::ObjectMapView(QObject *parent):QObject(parent),m_visible(true)
{
    QObject::connect(this,&ObjectMapView::visibleChanged,this,[=]()
                     {
        js_setVisible(m_visible);
                     });

}

