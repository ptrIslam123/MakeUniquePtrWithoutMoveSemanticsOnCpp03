#include <iostream>
#include "unique_ptr.h"
#include <string>
#include <vector>
#include <cassert>

struct Widget {
    Widget(const std::string id): id_(id) {
        std::cout << "\t\t**|| Construct widget: {" << id_ << "} ||**" << std::endl;
    }

    virtual ~Widget() {
        std::cout << "\t\t**|| Destruct widget: {" << id_ << "} ||**" << std::endl;
    }

    virtual void draw() = 0;

    const std::string& id() {
        return id_;
    }

private:
    std::string id_;
};


struct Button : public Widget {
    Button(const std::string &id): Widget(id) {}
    void draw() {
        std::cout << "Button::draw(): " << id() << std::endl;
    }
};

struct Window : public Widget {
    Window(const std::string &id): Widget(id) {}
    void draw() {
        std::cout << "Window::draw(): " << id() << std::endl;
    }
};

typedef mem::UniquePtr<Widget>::RvalueType RvalueSmartWidgetPtr;
typedef mem::UniquePtr<Widget> SmartWidgetPtr;
typedef std::vector<RvalueSmartWidgetPtr> Widgets;

RvalueSmartWidgetPtr makeWidget(const std::string &widgetType, const std::string &widgetId) {
    if (widgetType == "Button") {
        return SmartWidgetPtr(new Button(widgetId)).move();
    } else if (widgetType == "Window") {
        return SmartWidgetPtr(new Window(widgetId)).move();
    } else {
        return SmartWidgetPtr();
    }
}

void drawWidgets(const Widgets &widgets) {
    for (size_t i = 0; i < widgets.size(); ++i) {
        SmartWidgetPtr(widgets[i])->draw();
    }
}


void test1() {
    Widgets widgets;

    widgets.push_back(makeWidget("Button", "btn1"));
    widgets.push_back(makeWidget("Button", "btn2"));
    widgets.push_back(makeWidget("Window", "main window"));

    drawWidgets(widgets);
}

void test2() {
    SmartWidgetPtr w1(new Button("btn1"));
    SmartWidgetPtr w2(w1.move());

    w2->draw();

    assert(w1.get() == NULL);
    assert(w2.get() != NULL);
}

int main(void) {

    test2();

    return 0;
}
