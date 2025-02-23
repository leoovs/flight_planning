# The Event System

In systems with complex state it is crucial to efficiently observe state changes
that we *are interested in*.
The current event system implementation allows to decouple observers from event
sources and transfer all the necessary data in the form of *events*.

In consists of three major components:
- [Events](#event)
- [Event queue](#event-queue)
- [Event dispatcher](#event-dispatcher)

And two minor components:
[event bus and event subscriber](#event-bus-and-event-subscriber).

## Event

In the current implementation, an event is described as a separate class that
has to derive from the base `editor::Event` class. Fields of the class contain
data related to the transferred message.

**Example**

```C++
enum class KeyCode { /*...*/ };

class KeyDownEvent : public editor::Event
{
public:
    KeyCode KeyCode;

    KeyDownEvent(KeyCode keyCode) : KeyCode(keyCode) {}
};
```

## Event queue

The *FIFO* data structure (a simple wrapper around `std::queue<T>`) that allows
to store different types of events using `std::unique_ptr<T>`. The unique
pointer semantic implies that event queue is responsible for event lifetime.

**Example**

```C++
editor::EventQueue q;
q.Enqueue(std::make_unique<KeyDownEvent>(KeyCode::A));
q.Enqueue(std::make_unique<KeyDownEvent>(KeyCode::B))

std::unique_ptr<Event> e1 = q.Dequeue(); // KeyCode::A
std::unique_ptr<Event> e2 = q.Dequeue(); // KeyCode::B
```

## Event dispatcher

A table that allows to establish a connection between a handler and a message.
Internally the dispatcher assigns each handler a unique ID. A couple of event
type and handler ID forms a compound key that is used to identify
subscriptions.

**Example**

```C++
bool OnKeyDown(const KeyDownEvent& event) 
{
    if (event.KeyDown == KeyDown::A)
    {
        // handle 'A' key.
        UAVPF_LOG(Application, Trace, "KeyCode::A!");
        return true;
    }

    // We are not interested in other keys, so leave event in the queue until
    // someone else handles it. 
    return false;
}

// ...

void Runtime::Initialize()
{
    editor::EventDispatcher dispatcher;
    editor::EventSubscription subscription;

    subscription = dispatcher.Subscribe(
        editor::EventCallback<KeyDownEvent>(&OnKeyDown));

    KeyDownEvent myKeyCodeEvent;
    dispatcher.Notify(myKeyCodeEvent);

    // When we are done we should unsubscribe the handler from the event.
    dispatcher.Unsubscribe(subscription);
}
```

## Event bus and event subscriber

On top of earlier mentioned components there are two wrappers that should be
used together.

The first one, event bus, provides a unified interface for both publishing events
to the event queue and executing the dispatch mechanism via the event dispatcher.

**Example**

```C++

void Runtime::Initialize()
{
    // Event bus only references an event queue and an event dispatcher
    editor::EventBus bus(queue, dispatcher);

    // Simply forwards this to dispatcher.
    editor::EventSubscription sub = bus.Subscribe(editor::EventHandler<KeyDownEvent>(&OnKeyDown));

    // Adds event to the queue. 
    bus.Publish<KeyDownEvent>(editor::EventPublishMode::Queued, KeyCode::A);
    // Immediately calls handlers without adding event to the queue.
    bus.Publish<KeyDownEvent>(editor::EventPublishMode::Immediate, KeyCode::B);
    // Forwards events stored in the queue to the handlers.
    bus.Dispatch();
}
```

Event subscriber manages subscriptions for the programmer.

**Example**

```C++
void Runtime::Initialize(editor::EventBus& bus)
{
    mSubscriber = editor::EventSubscriber(bus);

    mSubscriber
        .BeginClass(*this)
            .SubscribeMethod(&Runtime::OnKeyDown)
            .SubscribeMethod(&Runtime::OnDEMLoaded)
            .SubscribeMethod(&Runtime::OnAlgorithmFinished)
            // etc.
        .EndClass();
}
```
