#ifndef SINGLETON_H
#define SINGLETON_H
template <typename T>
class Singleton {
protected:
    static T* instance;

    Singleton() {}
    ~Singleton() {}

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T* getInstance() {
        if (!instance) {
            instance = new T();
        }
        return instance;
    }

    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }
};

template <typename T>
T* Singleton<T>::instance = nullptr;
#endif // SINGLETON_H