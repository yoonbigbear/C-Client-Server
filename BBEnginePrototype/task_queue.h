#pragma once

#include "strand.h"

class TaskQueue : public Asio::Strand
{
public:
    TaskQueue(asio::io_context& io_context) :Strand(io_context) {}


};