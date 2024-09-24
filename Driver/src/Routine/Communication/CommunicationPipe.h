#pragma once

typedef void(*CommunicateRoutine)(void*, size_t);

class CommunicationPipe {
public:
    explicit CommunicationPipe(const char* id) : id(id) {}
public:
    virtual long Install(CommunicateRoutine routine) = 0;
    // TODO: Uninstall
public:
    const char* ID() const {
        return id;
    }
private:
    const char* id;
};
