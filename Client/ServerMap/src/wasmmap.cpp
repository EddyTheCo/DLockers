#include "wasmmap.hpp"

#include <emscripten.h>
#include <emscripten/bind.h>


EM_JS(void, js_setVisible, (const bool visible), {
    if(visible)
    {
        document.getElementById("map").style.visibility = "visible";
        document.getElementById("qtrootDiv").style.visibility = "hidden";
    }
    else
    {
        document.getElementById("map").style.visibility = "hidden";
        document.getElementById("qtrootDiv").style.visibility = "visible";
    }
});

ObjectMapView::ObjectMapView(QObject *parent):QObject(parent),m_visible(true)
{
    QObject::connect(this,&ObjectMapView::visibleChanged,this,[=]()
                     {
        js_setVisible(m_visible);
                     });

}

