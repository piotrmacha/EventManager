// (c) 2020 Piotr Macha <me@piotrmacha.pl>
// This code is licensed under MIT license

#pragma once

#include <vector>
#include <mutex>

namespace Fay
{
    /**
     * Base virtual class for observers to allow dynamic cast.
     * Should not be inherited directly by client observer.
     */
    struct IBaseObserver
    {
        virtual ~IBaseObserver() {};
    };

    /**
     * Interface for single Observer implementation.
     * Should not be inherited directly by client. Use Observer<...Args> instead.
     */
    template <typename T>
    struct IObserver
    {
        virtual void OnEvent(const T& event) = 0;
    };

    /**
     * Interface to be inherited by client observers. It sets up BaseObserver and multiple IObservers.
     *
     * Example:
     *  class MyObserver : public Fay::Observer<Event1, Event2, Event3> {}
     */
    template <typename... Args>
    struct Observer : public IBaseObserver, public IObserver<Args>...
    {
    };

    /**
     * Base EventManager can be configured by template parameters:
     *  Container: std::vector compatibile storage for Observers
     *  Mutex: std::mutex compatibile mutex
     */
    template <typename Container = std::vector<IBaseObserver*>, typename Mutex = std::mutex>
    class BaseEventManager
    {
    public:
        /**
         * Registers the Observer in EventManager
         */
        void Subscribe(IBaseObserver* observer)
        {
            std::lock_guard<Mutex> lock(mObserversMutex);
            mObservers.push_back(observer);
        }

        /**
         * Unregisters the Observer
         */
        void Unsubscribe(IBaseObserver* observer)
        {
            std::lock_guard<Mutex> lock(mObserversMutex);
            for (auto it = mObservers.begin(); it != mObservers.end(); ++it)
            {
                if ((*it) == observer)
                {
                    mObservers.erase(it);
                    return;
                }
            }
        }

        /**
         * Dispatches event of type T to all observers that can process it.
         */
        template <typename T>
        void Dispatch(const T& event)
        {
            std::lock_guard<Mutex> lock(mObserversMutex);
            for (auto* ptr : mObservers)
            {
                IObserver<T>* observer = dynamic_cast<IObserver<T>*>(ptr);
                // If observer is null it means that it can't process T
                if (observer)
                {
                    observer->OnEvent(event);
                }
            }
        }

        /**
         * r-value version of Dispatch(const T&)
         */
        template <typename T>
        void Dispatch(T&& event)
        {
            Dispatch((const T&) event);
        }

    private:
        Container mObservers;
        Mutex mObserversMutex;
    };

    /**
     * No-operation mutex implementation for single-threaded BaseEventManager
     */
    struct NullMutex
    {
        void lock() {}
        void unlock() {}
    };

    /**
     * Alias for ST EventManager using NullMutex
     */
    template <typename Container = std::vector<IBaseObserver*>>
    using SingleThreadedBaseEventManager = BaseEventManager<Container, NullMutex>;
}
