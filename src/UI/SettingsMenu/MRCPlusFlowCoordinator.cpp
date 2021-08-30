#include "main.hpp"
#include "UI/SharedUIManager.hpp"
#include "UI/SettingsMenu/MRCPlusFlowCoordinator.hpp"
#include "UI/SettingsMenu/MRCPlusGraphicsView.hpp"
#include "UI/SettingsMenu/MRCPlusMainView.hpp"
#include "UI/SettingsMenu/MRCPlusCameraView.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"

#include "HMUI/ViewController.hpp"
#include "HMUI/ViewController_AnimationType.hpp"
#include "HMUI/ViewController_AnimationDirection.hpp"
#include "HMUI/FlowCoordinator.hpp"

using namespace MRCPlus;
DEFINE_TYPE(MRCPlus, MRCPlusFlowCoordinator);

void MRCPlusFlowCoordinator::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    MRCPlusSettingsFC = (HMUI::FlowCoordinator*)this;
    if (firstActivation) 
    {
        this->SetTitle(il2cpp_utils::newcsstr("MRCPlus"), (int)1);
        this->showBackButton = true;

        if (!this->mrcMainView) this->mrcMainView = QuestUI::BeatSaberUI::CreateViewController<MRCPlusMainView*>();
        if (!this->mrcCameraView) this->mrcCameraView = QuestUI::BeatSaberUI::CreateViewController<MRCPlusCameraView*>();
        if (!this->mrcGraphicsView) this->mrcGraphicsView = QuestUI::BeatSaberUI::CreateViewController<MRCPlusGraphicsView*>();
        MRCPlusFlowCoordinator::ProvideInitialViewControllers(mrcMainView, mrcCameraView, mrcGraphicsView, nullptr, nullptr);
    }
}

void MRCPlusFlowCoordinator::UpdateSettingsVisibility(bool leftVisible, bool rightVisible)
{
    MRCPlusCameraView* cameraVC = (MRCPlusCameraView*)this->mrcCameraView;
    MRCPlusGraphicsView* graphicsVC = (MRCPlusGraphicsView*)this->mrcGraphicsView;
    cameraVC->OnChangeVisibility(leftVisible);
    graphicsVC->OnChangeVisibility(rightVisible);
}

void MRCPlusFlowCoordinator::BackButtonWasPressed(HMUI::ViewController* topView)
{
    getConfig().Write();
    HMUI::FlowCoordinator* settingsFC = QuestUI::GetModSettingsFlowCoordinator();
    settingsFC->DismissFlowCoordinator(this, (int)0, nullptr, false);
}