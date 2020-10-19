#pragma once

#include <memory>
#include <mutex>

static std::mutex m;

template <class T> class Singleton {
protected:
    static inline std::shared_ptr<T> instance{nullptr};

public:
  static std::shared_ptr<T> getInstance() {
      std::unique_lock lk{m};
      if (instance.get() == nullptr) {
          instance = std::shared_ptr<T>(new T());
      }
      return instance;

  }
};

template <class T, class... Args> class ParamSingleton {

protected:
    static inline std::shared_ptr<T> instance{nullptr};

public:
  static std::shared_ptr<T> getInstance(Args... args) {
      std::unique_lock lk{m};
      if (instance.get() == nullptr) {
          instance = std::shared_ptr<T>(new T(args...));
      }
      return instance;

  }
};