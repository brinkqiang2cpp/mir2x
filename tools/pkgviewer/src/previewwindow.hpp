// generated by Fast Light User Interface Designer (fluid) version 1.0303

#pragma once
#include <FL/Fl_Double_Window.H>
#include <cstdint>

class PreviewWindow: public Fl_Double_Window
{
    public:
        PreviewWindow(int, int);
        ~PreviewWindow();
    public:
        void draw();
    private:
        uint32_t *m_Data;
        int       m_DataLen;
};