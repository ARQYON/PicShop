#pragma once
#include <memory>
#include <stdexcept>
#include <vector>
#include "Image.h"

/* History - undo/redo system for PicShop.
   Uses full image snapshots. Every time the user performs an action, call
   push(currentImage) to save the state. Call undo() to go back. Call
   redo() to go forward.
   maxStates controls memory usage. Default 20 states matches most
   professional editors. */
class History
{
private:
    int m_maxStates;
    std::vector<std::unique_ptr<Image>> m_undoStack;
    std::vector<std::unique_ptr<Image>> m_redoStack;

public:
    // Construction
    explicit History(int maxStates = 20)
        : m_maxStates(maxStates)
    {
        if (maxStates < 1)
            throw std::invalid_argument("History: maxStates must be at least 1.");
    }

    /* Push a new state.
       Call this before every operation so the state before the change is
       saved. Then apply your operation. */
    void push(const Image &img)
    {
        // Any new action wipes the redo stack entirely.
        m_redoStack.clear();
        // Save a copy of the current image
        m_undoStack.push_back(std::make_unique<Image>(img));
        // If we exceed the limit, drop the oldest state.
        if (static_cast<int>(m_undoStack.size()) > m_maxStates)
            m_undoStack.erase(m_undoStack.begin());
    }

    /* Undo
       Returns the previous image state. Call with your current image so
       it can be saved to redo. */
    Image undo(const Image &current)
    {
        if (m_undoStack.empty())
            throw std::runtime_error("History: Nothing to undo.");
        // Save current state to redo stack.
        m_redoStack.push_back(std::make_unique<Image>(current));
        // Pop and return the last saved state.
        Image previous = *m_undoStack.back();
        m_undoStack.pop_back();
        return previous;
    }

    /* Redo
       Returns the state that was undone. Call with your current image so
       it can be saved to undo. */
    Image redo(const Image &current)
    {
        if (m_redoStack.empty())
            throw std::runtime_error("History: Nothing to redo.");
        // Save current state back to undo stack
        m_undoStack.push_back(std::make_unique<Image>(current));
        // Pop and return the redo state
        Image next = *m_redoStack.back();
        m_redoStack.pop_back();
        return next;
    }

    // State queries.
    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }

    int undoCount() const
    {
        return static_cast<int>(m_undoStack.size());
    }

    /* Clear all history.
       Call when opening a new file or creating a new document. */
    void clear()
    {
        m_undoStack.clear();
        m_redoStack.clear();
    }

    /* Memory usage estimate.
       Returns approximate bytes used by stored history states. */
    size_t memoryUsage() const
    {
        size_t total = 0;
        for (const auto &img : m_undoStack)
            total += img->bytesSize();
        for (const auto &img : m_redoStack)
            total += img->bytesSize();
        return total;
    }
};