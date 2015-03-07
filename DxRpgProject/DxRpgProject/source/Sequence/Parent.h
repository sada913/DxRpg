#pragma once
#include "Child.h"
#include "Sequence/Game/Parent.h"

namespace Sequence
{
class Parent
{
public:
    enum NextSequence
    {
        NextMap,
        NextNone,
    };

    void update()
    {
        child_->update(this);
        switch (nextSequence_)
        {
        case NextMap:
            if (child_ != NULL)
            {
                delete child_;
            }
            child_ = new Game::Parent();
            break;
        case NextNone:
            break;
        default:
            exit(EXIT_FAILURE);
            break;
        }
        nextSequence_ = NextNone;
    }

    void moveTo(NextSequence next)
    {
        nextSequence_ = next;
    }

    static Parent* getInstance()
    {
        static Parent parent;
        return &parent;
    }

private:
    Parent()
        : nextSequence_(NextNone),
        child_(NULL)
    {
        child_ = new Game::Parent();
    }

    ~Parent()
    {
        if (child_ != NULL)
        {
            delete child_;
        }
    }

    Parent(const Parent&) { }
    Parent& operator=(const Parent&) { }
    NextSequence nextSequence_;
    Child *child_;
};
}
