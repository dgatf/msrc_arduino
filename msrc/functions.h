#ifndef FUNCTIONS_H
#define FUNCITONS_H

#include <Arduino.h>

#include "constants.h"
#include "softserial.h"
#include "hardserial.h"

uint8_t setCellCount(float voltage);
float calcAverage(float alpha, float oldValue, float newValue);

class Consumption
{
private:
    uint16_t prevMs = 0;

public:
    float calcConsumption(float current, uint16_t currentMax = 0);
};

class Vario
{
private:
    uint16_t prevMs = 0;
    float prevAltitude = 0;
    float speed = 0;

public:
    float calcSpeed(float altitude, uint16_t intervalMin = 0);
    float calcAltitude(float pressure, float temperature, float P0);
};

template <typename T>
class CircularBuffer
{
private:
    struct Node
    {
        T *itemP = NULL;
        Node *nextP = NULL;
    };
    Node *nodeP = NULL;

public:
    CircularBuffer() { nodeP = NULL; };
    ~CircularBuffer()
    {
        empty();
    };
    void add(T *itemP)
    {
        Node *newNodeP;
        newNodeP = new Node;
        newNodeP->itemP = itemP;
        if (nodeP == NULL)
        {
            nodeP = newNodeP;
            newNodeP->nextP = newNodeP;
        }
        else
        {
            newNodeP->nextP = nodeP->nextP;
            nodeP->nextP = newNodeP;
            nodeP = newNodeP;
        }
    };
    T *current()
    {
        if (nodeP)
            return nodeP->itemP;
        return NULL;
    };
    void next()
    {
        if (nodeP)
            nodeP = nodeP->nextP;
    };
    void empty()
    {
        if (nodeP)
        {
            Node *firstNodeP, *nextNodeP;
            firstNodeP = nodeP;
            do
            {
                nextNodeP = nodeP->nextP;
                delete nodeP->itemP;
                delete nodeP;
                nodeP = nextNodeP;
            } while (nextNodeP != firstNodeP);
            nodeP = NULL;
        }
    };
};

template <typename T>
class FifoBuffer
{
private:
    struct Node
    {
        T *itemP = NULL;
        Node *nextP = NULL;
    };
    Node *firstNodeP = NULL;
    Node *lastNodeP = NULL;

public:
    FifoBuffer(){};
    ~FifoBuffer()
    {
        empty();
    };
    void add(T *itemP)
    {
        Node *newNodeP;
        newNodeP = new Node;
        newNodeP->itemP = itemP;
        if (firstNodeP == NULL)
        {
            firstNodeP = newNodeP;
            lastNodeP = newNodeP;
        }
        else
        {
            lastNodeP->nextP = newNodeP;
            lastNodeP = newNodeP;
        }
    };
    T *current()
    {
        if (firstNodeP)
            return firstNodeP->itemP;
        return NULL;
    };
    bool drop()
    {
        if (firstNodeP)
        {
            Node *tmpNodeP = firstNodeP;
            firstNodeP = firstNodeP->nextP;
            delete tmpNodeP->itemP;
            delete tmpNodeP;
            return true;
        }
        return false;
    };
    void empty()
    {
        while (drop())
            ;
    };
};

#endif