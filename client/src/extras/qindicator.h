#pragma once

#include <QLabel>

class QIndicator : public QLabel
{
public:
    QIndicator(QWidget* parent = nullptr) : QLabel("", parent) {  }
    ~QIndicator() {  }

public:
    enum class State
    {
        RED, GREEN, YELLOW
    };

    inline void setState(State state)
    {
        this->state = state;

        switch (state)
        {
        case State::RED:
            setStyleSheet(R"(QLabel {
	                        color: white;
                            border-radius: 5px;
                            background-color: qlineargradient(spread:pad, x1:0.145, y1:0.16, x2:1, y2:1, 
                                                              stop:0 rgba(252, 20, 7, 255), 
                                                              stop:1 rgba(134, 25, 5, 255)); })");
            break;

        case State::GREEN:
            setStyleSheet(R"(QLabel {
	                        color: white;
                            border-radius: 5px;
                            background-color: qlineargradient(spread:pad, x1:0.145, y1:0.16, x2:1, y2:1, 
                                                              stop:0 rgba(20, 252, 7, 255), 
                                                              stop:1 rgba(25, 134, 5, 255)); })");
            break;

        case State::YELLOW:
            setStyleSheet(R"(QLabel {
	                        color: white;
                            border-radius: 5px;
                            background-color: qlineargradient(spread:pad, x1:0.145, y1:0.16, x2:1, y2:1, 
                                                              stop:0 rgba(251, 250, 5, 255), 
                                                              stop:1 rgba(251, 211, 27, 255)); })");
            break;
        
        default:
            break;
        }
    }

    inline State getState()
    {
        return state;
    }

private:
    State state;
};