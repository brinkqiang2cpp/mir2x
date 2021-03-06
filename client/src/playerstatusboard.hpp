/*
 * =====================================================================================
 *
 *       Filename: playerstatusboard.hpp
 *        Created: 10/08/2017 19:06:52
 *    Description:
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#pragma once
#include "widget.hpp"
#include "labelboard.hpp"
#include "tritexbutton.hpp"

class ProcessRun;
class PlayerStatusBoard: public Widget
{
    private:
        TritexButton m_closeButton;

    private:
        std::vector<TritexButton *> m_elemStatusList;

    private:
        ProcessRun *m_processRun;

    public:
        PlayerStatusBoard(int, int, ProcessRun *, Widget * = nullptr, bool = false);

    public:
        void update(double) override;
        void drawEx(int, int, int, int, int, int) override;
        bool processEvent(const SDL_Event &, bool) override;
};
