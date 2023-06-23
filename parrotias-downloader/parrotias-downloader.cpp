#include <wx/wx.h>
#include <wx/filename.h>
#include "logo_data.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Downloader : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(Downloader);

class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    //void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    wxBoxSizer* CreateMainSizer();
};

enum
{
};

bool Downloader::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Parrotias setup", wxDefaultPosition, wxSize(1000, 300), wxDEFAULT_FRAME_STYLE)
{
    SetSizer(CreateMainSizer());

    Fit();

    Centre();

    SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a Parrotias Windows app downloader ",
        "About Parrotias Downloader", wxOK | wxICON_INFORMATION);
}


wxBoxSizer* MyFrame::CreateMainSizer()
{
    // Create the main sizer that will contain the logo, and subsizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Create sub sizer to contain messages and progress bar
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);


    // Load the image data using stb_image
    int width, height, channels;
    unsigned char* image_data = stbi_load_from_memory(logo_data, logo_data_size, &width, &height, &channels, 0);

    if (image_data)
    {
        wxImage image(width, height, image_data, true);
        wxBitmap logoBitmap(image);
        wxStaticBitmap* logo = new wxStaticBitmap(this, wxID_ANY, logoBitmap);
        mainSizer->Add(logo, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);

        stbi_image_free(image_data);
    }
    else
    {
        // Failed to load image data
        wxMessageBox("Failed to load image", "Error", wxOK | wxICON_ERROR);
    }

    // Create title message
    wxStaticText* titleMessage = new wxStaticText(this, wxID_ANY, "Parrotias");
    rightSizer->Add(titleMessage, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    // Create the progress bar
    wxGauge* progressBar = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(400, 10), wxGA_HORIZONTAL);
    rightSizer->Add(progressBar, 0, wxALL | wxEXPAND, 10);

    // Create downloading message
    wxStaticText* downloadMessage = new wxStaticText(this, wxID_ANY, "Installing Parrotias...");
    rightSizer->Add(downloadMessage, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    // Place Subsizer to MainSizer
    mainSizer->Add(rightSizer, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);

    // Set the main sizer as the sizer for the frame
    SetSizer(mainSizer);

    return mainSizer;
}