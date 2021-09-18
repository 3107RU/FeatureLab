#pragma once

#include <deque>
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <functional>
#include <filesystem>

class Dataset
{
public:
    ~Dataset();
    struct Image
    {
        int index;
        std::string name;
        std::wstring file;
        struct Box
        {
            int top, left, width, height;
            struct Part
            {
                int x, y;
            };
            std::vector<Part> parts;
        };
        std::vector<Box> boxes;
    };
    struct LoadResult
    {
        enum
        {
            TypeStarted,
            TypeImage,
            TypeError,
            TypeSuccess
        };
        int type;
        std::string error;
        std::shared_ptr<Image> image;
    };
    bool load(std::wstring path, std::function<void(LoadResult)> cb);
    struct GetResult
    {
        std::vector<std::shared_ptr<Image>> images;
        int morebefore, moreafter;
    };
    GetResult get(int start, int count);
    int count();
    bool stop(bool wait);
    bool save(std::shared_ptr<Image> image);
    bool remove(std::wstring path);
    void savePts();
    bool exportXml(std::wstring path);
    bool importFromiBUG(std::wstring path, std::function<void(LoadResult)> cb);

private:
    volatile bool active = false, busy = false;
    std::shared_ptr<std::thread> worker;
    std::mutex mtx;
    using Lock = std::lock_guard<std::mutex>;
    std::filesystem::path current;
    std::deque<std::shared_ptr<Image>> images;
};