#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <sciter-x-graphics.hpp>
#include <sciter-x-video-api.h>
#include <sciter-win-main.cpp>

#include "dataset.h"

class MainWindow : public sciter::window
{
    Dataset dataset;

    bool open(std::wstring path, sciter::value cb)
    {
        return dataset.load(
            path,
            [cb](Dataset::LoadResult result)
            {
                auto data = sciter::value::make_map();
                data.set_item("type", result.type);
                if (result.type == Dataset::LoadResult::TypeError)
                    data.set_item("error", result.error);
                cb.call(data);
            });
    }

    sciter::value get(sciter::value start, int count)
    {
        int begin = start.is_undefined() ? (count < 0 ? dataset.count() : 0) : start.get(0);
        auto result = dataset.get(begin, count);
        auto data = sciter::value::make_map();
        data.set_item("morebefore", result.morebefore);
        data.set_item("moreafter", result.moreafter);
        auto images = sciter::value::make_array();
        for (size_t k = 0; k < result.images.size(); k++)
        {
            auto img = result.images[k];
            auto image = sciter::value::make_map();
            image.set_item("index", img->index);
            image.set_item("name", img->name);
            image.set_item("file", img->file);
            auto boxes = sciter::value::make_array();
            for (size_t i = 0; i < img->boxes.size(); i++)
            {
                auto box = sciter::value::make_map();
                box.set_item("left", img->boxes[i].left);
                box.set_item("top", img->boxes[i].top);
                box.set_item("width", img->boxes[i].width);
                box.set_item("height", img->boxes[i].height);
                auto parts = sciter::value::make_array();
                for (size_t j = 0; j < img->boxes[i].parts.size(); j++)
                {
                    auto part = sciter::value::make_map();
                    part.set_item("x", img->boxes[i].parts[j].x);
                    part.set_item("y", img->boxes[i].parts[j].y);
                    parts.set_item(int(j), part);
                }
                box.set_item("parts", parts);
                boxes.set_item(int(i), box);
            }
            image.set_item("boxes", boxes);
            images.set_item(int(k), image);
        }
        data.set_item("images", images);
        return data;
    }

    bool stop()
    {
        return dataset.stop(false);
    }

    bool remove(std::wstring path)
    {
        return dataset.remove(path);
    }

    bool save(sciter::value data)
    {
        auto image = std::make_shared<Dataset::Image>();
        image->name = aux::w2utf(data.get_item("name").get(L""));
        image->file = data.get_item("file").get(L"");
        auto boxes = data.get_item("boxes");
        for (int i = 0; i < boxes.length(); i++)
        {
            Dataset::Image::Box box;
            box.left = boxes.get_item(i).get_item("left").get(0);
            box.top = boxes.get_item(i).get_item("top").get(0);
            box.width = boxes.get_item(i).get_item("width").get(0);
            box.height = boxes.get_item(i).get_item("height").get(0);
            auto parts = boxes.get_item(i).get_item("parts");
            for (int j = 0; j < parts.length(); j++)
            {
                Dataset::Image::Box::Part part;
                part.x = parts.get_item(j).get_item("x").get(0);
                part.y = parts.get_item(j).get_item("y").get(0);
                box.parts.push_back(part);
            }
            image->boxes.push_back(box);
        }
        return dataset.save(image);
    }

    void savePts()
    {
        return dataset.savePts();
    }

    bool exportXml(std::wstring path)
    {
        return dataset.exportXml(path);
    }

    bool importFromiBUG(std::wstring path, sciter::value cb)
    {
        return dataset.importFromiBUG(
            path,
            [cb](Dataset::LoadResult result)
            {
                auto data = sciter::value::make_map();
                data.set_item("type", result.type);
                if (result.type == Dataset::LoadResult::TypeError)
                    data.set_item("error", result.error);
                cb.call(data);
            });
    }

public:
    MainWindow() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_GLASSY) {}
    SOM_PASSPORT_BEGIN_EX(api, MainWindow)
    SOM_FUNCS(SOM_FUNC(open), SOM_FUNC(get), SOM_FUNC(stop), 
    SOM_FUNC(remove), SOM_FUNC(save), SOM_FUNC(exportXml), 
    SOM_FUNC(importFromiBUG), SOM_FUNC(savePts))
    SOM_PASSPORT_END
};

#include "resources.cpp"

int uimain(std::function<int()> run)
{
    SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
                    ALLOW_FILE_IO | ALLOW_SOCKET_IO | ALLOW_EVAL | ALLOW_SYSINFO);
#ifndef NDEBUG
    SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);
#endif
    try
    {
        sciter::archive::instance().open(aux::elements_of(resources));
        sciter::om::hasset<MainWindow> pwin(new MainWindow());
        pwin->load(WSTR("this://app/main.htm"));
        pwin->expand();
        return run();
    }
    catch (const std::exception &e)
    {
        MessageBoxA(NULL, e.what(), "Unexpected exception!", MB_ICONSTOP | MB_OK);
        return 1;
    }
    return 0;
}
