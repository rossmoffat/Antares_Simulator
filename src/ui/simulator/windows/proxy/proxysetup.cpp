/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "proxysetup.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>

#include "../../toolbox/create.h"
#include "../../toolbox/resources.h"
#include "../../application/menus.h"
#include "../message.h"
#include "../../../../internet/license.h"

#include <ui/common/component/panel.h>
#include <antares/logs.h>
#include "../startupwizard.h"


using namespace Yuni;


#ifdef YUNI_OS_WINDOWS
# define FIXED_FONTNAME wxT("Consolas")
# define FIXED_FONTNAME_FALLBACK wxT("Lucida Console")
# define FIXED_FONT_SIZE 0
#else
# define FIXED_FONTNAME wxT("Monospace")
# define FIXED_FONTNAME_FALLBACK wxT("Monospace")
# define FIXED_FONT_SIZE -1
#endif





namespace Antares
{
namespace Window
{
	
	LicenseCoultNotConnectToInternetServer::LicenseCoultNotConnectToInternetServer(wxWindow* parent) :
		wxDialog(parent, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize,
			wxCLOSE_BOX|wxCAPTION|wxCLIP_CHILDREN),
		pEditProxyHost(nullptr),
		pEditProxyPort(nullptr),
		pEditProxyLogin(nullptr),
		pEditProxyPass(nullptr),
		pCanceled(true)
	{
		assert(parent);

		
		// Background color
		wxColour defaultBgColor = GetBackgroundColour();
		wxColour bgColor(255, 255, 255);
		SetBackgroundColour(bgColor);

		auto* sizer = new wxBoxSizer(wxVERTICAL);
		sizer->AddSpacer(30);

		auto* hz = new wxBoxSizer(wxHORIZONTAL);
		hz->AddSpacer(30);
		hz->Add(Resources::StaticBitmapLoadFromFile(this, wxID_ANY, "images/64x64/server.png"), 0, wxALL|wxALIGN_TOP|wxALIGN_CENTER_HORIZONTAL);
		sizer->Add(hz, 1, wxALL|wxEXPAND);

		auto* contentSizer = new wxBoxSizer(wxVERTICAL);
		hz->Add(contentSizer, 1, wxALL|wxEXPAND);
		hz->AddSpacer(30);

		auto* titlespacer = new wxBoxSizer(wxHORIZONTAL);
		auto* title  = Component::CreateLabel(this,
			wxT("Impossible to connect to the internet"), false, false, +1);
		auto* subtitle  = Component::CreateLabel(this,
			wxT("No internet access or connection controlled by local proxy.\nPlease fill out your proxy parameters to allow connection."), false, false);
		subtitle->Enable(false);
		contentSizer->AddSpacer(20);
		contentSizer->Add(titlespacer, 0, wxALL|wxEXPAND);
		contentSizer->AddSpacer(40);


		auto* subtitlespacer = new wxBoxSizer(wxVERTICAL);
		subtitlespacer->AddStretchSpacer();
		subtitlespacer->Add(title, 0, wxALIGN_CENTER_VERTICAL|wxLEFT);
		subtitlespacer->AddSpacer(3);
		subtitlespacer->Add(subtitle, 0, wxALIGN_CENTER_VERTICAL|wxLEFT);
		subtitlespacer->AddStretchSpacer();
		titlespacer->AddSpacer(10);
		titlespacer->Add(subtitlespacer, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxLEFT);


		auto* proxySizerDiv = new wxBoxSizer(wxHORIZONTAL);
		proxySizerDiv->AddSpacer(10);
		auto* proxySizer = new wxBoxSizer(wxVERTICAL);
		proxySizerDiv->Add(proxySizer, 1, wxALL|wxEXPAND);
		contentSizer->Add(proxySizerDiv, 1, wxALL|wxEXPAND);

		pFlexSizer = new wxFlexGridSizer(2, 2, 5);
		auto* loginSizer = new wxBoxSizer(wxHORIZONTAL);
		//loginSizer->AddSpacer(25);
		loginSizer->Add(pFlexSizer, 1, wxALL|wxEXPAND);
		proxySizer->Add(loginSizer, 1, wxALL|wxEXPAND);

		pEditProxyHost  = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(170, -1));
		pEditProxyPort  = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(60, -1));
		pEditProxyLogin = new wxTextCtrl(this, wxID_ANY, wxT(""));
		pEditProxyPass  = new wxTextCtrl(this, wxID_ANY, wxT(""),wxDefaultPosition,wxSize(170, -1),wxTE_PASSWORD);

		pLblProxyHost   = Component::CreateLabel(this, wxT("Proxy IP address:"));
		pLblProxyPort   = Component::CreateLabel(this, wxT("Port:"));
		pLblProxyLogin  = Component::CreateLabel(this, wxT("Username:"));
		pLblProxyPass   = Component::CreateLabel(this, wxT("Password:"));

		// display proxy parameters
		auto& proxy = Antares::License::proxy;
		// read proxy parameters from file
		proxy.loadProxyFile();
		pEditProxyHost->SetValue(wxStringFromUTF8(proxy.host));
		pEditProxyPort->SetValue(wxStringFromUTF8(proxy.port));
		pEditProxyLogin->SetValue(wxStringFromUTF8(proxy.login));
		pEditProxyPass->SetValue(wxStringFromUTF8(proxy.password));
		

		auto* urlDiv = new wxBoxSizer(wxHORIZONTAL);
		urlDiv->Add(pEditProxyHost, 1, wxALL|wxEXPAND);
		urlDiv->AddSpacer(10);
		urlDiv->Add(pLblProxyPort, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxLEFT);
		urlDiv->AddSpacer(5);
		urlDiv->Add(pEditProxyPort, 0, wxALL|wxEXPAND);

		pFlexSizer->Add(pLblProxyHost, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxLEFT);
		pFlexSizer->Add(urlDiv, 1, wxALL|wxEXPAND);
		pFlexSizer->Add(pLblProxyLogin, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxLEFT);
		pFlexSizer->Add(pEditProxyLogin, 1, wxALL|wxEXPAND);
		pFlexSizer->Add(pLblProxyPass, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxLEFT);
		pFlexSizer->Add(pEditProxyPass, 1, wxALL|wxEXPAND);

		contentSizer->AddSpacer(40);

		// Buttons
		auto* panel = new Component::Panel(this);
		panel->SetBackgroundColour(defaultBgColor);
		wxBoxSizer* pnlSizerBtns = new wxBoxSizer(wxHORIZONTAL);
		wxBoxSizer* pnlSizerBtnsV = new wxBoxSizer(wxVERTICAL);
		panel->SetSizer(pnlSizerBtnsV);
		pnlSizerBtnsV->AddSpacer(8);
		pnlSizerBtnsV->Add(pnlSizerBtns, 1, wxALL|wxEXPAND);
		pnlSizerBtnsV->AddSpacer(8);

		pnlSizerBtns->AddSpacer(25);

		pnlSizerBtns->AddStretchSpacer();
		auto* pBtnValidate = Antares::Component::CreateButton(panel, wxT(" Connect "), this, &LicenseCoultNotConnectToInternetServer::onProceed);
		pBtnValidate->SetDefault();


		auto* pBtnCancel = Antares::Component::CreateButton(panel, wxT(" Cancel "), this, &LicenseCoultNotConnectToInternetServer::onClose);

		pnlSizerBtns->Add(pBtnCancel, 0, wxALL|wxEXPAND);
		pnlSizerBtns->AddSpacer(5);
		pnlSizerBtns->Add(pBtnValidate, 0, wxALL|wxEXPAND);
		pnlSizerBtns->Add(25, 5);

		sizer->Add(new wxStaticLine(this, wxID_ANY), 0, wxALL|wxEXPAND);
		sizer->Add(panel, 0, wxALL|wxEXPAND);

		// refresh
		Connect(GetId(), wxEVT_MOTION,
			wxMouseEventHandler(LicenseCoultNotConnectToInternetServer::onInternalMotion), NULL, this);



		SetSizer(sizer);
		sizer->Layout();

		// Recentre the window
		sizer->Fit(this);
		Centre(wxBOTH);

	}


	LicenseCoultNotConnectToInternetServer::~LicenseCoultNotConnectToInternetServer()
	{
		//MakeModal(false);
		Component::Spotlight::FrameClose();
	}


	void LicenseCoultNotConnectToInternetServer::onClose(void*)
	{
		Dispatcher::GUI::Close(this);
	}


	void LicenseCoultNotConnectToInternetServer::onInternalMotion(wxMouseEvent&)
	{
		Component::Panel::OnMouseMoveFromExternalComponent();
	}


	void LicenseCoultNotConnectToInternetServer::onProceed(void*)
	{
		auto& proxy = Antares::License::proxy;
		// update proxy value
		proxy.enabled = true;
		if (proxy.enabled)
		{
			wxStringToString(pEditProxyHost->GetValue(), proxy.host);
			wxStringToString(pEditProxyLogin->GetValue(), proxy.login);
			wxStringToString(pEditProxyPass->GetValue(), proxy.password);
			wxStringToString(pEditProxyPort->GetValue(), proxy.port);
		}

		// busy !
		::wxBeginBusyCursor();

		// check if the proxy is correct
		if(!proxy.check())
		{
			logs.error() << "Incorrect or missing proxy settings, please check your proxy settings";
			::wxEndBusyCursor();
			return;
		}

		// if check license on the server failed
		if(not Antares::License::CheckOnlineLicenseValidity(Data::versionLatest, true))
		{
			String errMessage;
			String errType;
			Antares::License::WriteLastError(errMessage, errType);


			wxString wxMessage = wxString(errMessage.c_str(), wxConvUTF8);
			wxString wxType = wxString(errType.c_str(), wxConvUTF8);

			// dispaly error message
			Antares::Window::Message message(this, wxT(""),
				//wxT("Error with the license Server"),
				wxType,
				wxMessage,
				"images/misc/error.png");
			message.add(Window::Message::btnContinue);
			message.showModal();

			::wxEndBusyCursor();
			return;
		}

		//save parameters in the file
		proxy.saveProxyFile();

		::wxEndBusyCursor();

		pCanceled = false;
		// finish - let's continue !
		Dispatcher::GUI::Close(this);
		// display the next window (welcome page or proxy setup page)
		StartupWizard::Show();
	}


	




	bool LicenseCoultNotConnectToInternetServer::canceled() const
	{
		return pCanceled;
	}


} // namespace Window
} // namespace Antares
