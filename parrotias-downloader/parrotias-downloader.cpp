#include <wx/wx.h>
#include <wx/filename.h>
#include <curl/curl.h>
#include <zip.h>
#include <cstdio>
#include <cstdlib>
#include <wx/stdpaths.h>
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
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSetStatusInstallText(wxCommandEvent& event);

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
    SetBackgroundColour(wxColour(240, 240, 240));
    
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

void MyFrame::OnSetStatusInstallText(wxCommandEvent& event)
{
    std::cout << "Install Status Changed";
}

// Function to write downloaded data to a file
size_t WriteData(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    return fwrite(ptr, size, nmemb, stream);
}


// Function to download a file from an HTTP source
bool DownloadFile(const wxString& url, const wxString& destination)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();
    if (curl)
    {
        FILE* file = fopen(destination.c_str(), "wb");
        if (file)
        {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

            CURLcode result = curl_easy_perform(curl);

            fclose(file);

            if (result != CURLE_OK)
            {
                wxMessageBox(wxString::Format("Failed to download file: %s", curl_easy_strerror(result)), "Error", wxOK | wxICON_ERROR);
                return false;
            }
        }
        else
        {
            wxMessageBox(wxString::Format("Failed to create file: %s", destination), "Error", wxOK | wxICON_ERROR);
            return false;
        }

        curl_easy_cleanup(curl);
        return true;
    }

    wxMessageBox("Failed to initialize libcurl", "Error", wxOK | wxICON_ERROR);
    return false;
}

// Function to unzip a file
bool UnzipFile(const wxString& zipFile, const wxString& destination)
{
    int err = 0;
    zip* archive = zip_open(zipFile.mb_str(), 0, &err);
    if (archive)
    {
        int numFiles = zip_get_num_entries(archive, 0);
        for (int i = 0; i < numFiles; ++i)
        {
            zip_stat_t fileStat;
            zip_stat_index(archive, i, 0, &fileStat);

            zip_file_t* file = zip_fopen_index(archive, i, 0);
            if (file)
            {
                wxString filePath = wxString::Format("%s/%s", destination, fileStat.name);
                FILE* outFile = fopen(filePath.mb_str(), "wb");
                if (outFile)
                {
                    char* buffer = new char[fileStat.size];
                    zip_fread(file, buffer, fileStat.size);
                    fwrite(buffer, fileStat.size, 1, outFile);
                    delete[] buffer;

                    fclose(outFile);
                }

                zip_fclose(file);
            }
        }

        zip_close(archive);
        return true;
    }

    wxMessageBox(wxString::Format("Failed to open zip file: %s", zipFile), "Error", wxOK | wxICON_ERROR);
    return false;
}

// Download and unzip usage
void DownloadAndUnzipFile()
{
    wxString url = "https://github.com/Steelzen/parrotias-windows/archive/refs/tags/v0.5.0.zip";

    wxFileName documentsDir(wxStandardPaths::Get().GetDocumentsDir());
    wxString destination = documentsDir.GetPath() + wxFileName::GetPathSeparator() + "Parrotias";
    wxString zipFile = destination + wxFileName::GetPathSeparator() + "parrotias-windows-0.5.0.zip";

    // Create the destination directory if it doesn't exist
    if (!wxDirExists(destination))
    {
        if (!wxMkdir(destination))
        {
            wxMessageBox("Failed to create destination directory", "Error", wxOK | wxICON_ERROR);
            return;
        }
    }

    // Download the file
    if (DownloadFile(url, zipFile))
    {
        // Unzip the downloaded file
        if (UnzipFile(zipFile, destination))
        {
            // Delete the zip file
            if (std::remove(zipFile.mb_str()) == 0)
            {
                wxMessageBox("File downloaded, unzipped, and zip file deleted successfully!", "Success", wxOK | wxICON_INFORMATION);
            }
            else
            {
                wxMessageBox("Failed to delete zip file", "Error", wxOK | wxICON_ERROR);
            }
        }
    }
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
    wxFont font(wxFontInfo(14).Bold());
    titleMessage->SetFont(font);
    rightSizer->Add(titleMessage, 0, wxALL | wxALIGN_LEFT, 10);

    // Under message
    wxStaticText* customMessage = new wxStaticText(this, wxID_ANY, "Can't wait you will use it!", wxDefaultPosition, wxSize(400, 100), wxGA_HORIZONTAL);
    rightSizer->Add(customMessage, 0, wxALL | wxALIGN_LEFT, 10);

    // Create the progress bar
    wxGauge* progressBar = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(400, 10), wxGA_HORIZONTAL);
    rightSizer->Add(progressBar, 0, wxALL | wxEXPAND, 10);

    // Create downloading message
    wxStaticText* downloadMessage = new wxStaticText(this, wxID_ANY, "Installing Parrotias...");
    rightSizer->Add(downloadMessage, 0, wxALL | wxALIGN_LEFT, 10);

    // Place Subsizer to MainSizer
    mainSizer->Add(rightSizer, 0, wxALL | wxALIGN_CENTER_VERTICAL, 10);

    // Set the main sizer as the sizer for the frame
    SetSizer(mainSizer);

    //DownloadAndUnzipFile();

    return mainSizer;
}