#include <memory>

#include "standard-game/BeStandardGame.h"
#include "standard-game/BeStandardBaseScene.h"
#include "scenes/BeSceneManager.h"

#include "RiftScene.h"

int main() {
    BeStandardGame game({
        .Title = "rift",
        .WindowMode = BeWindowMode::Fullscreen,
    });

    auto& scenes = *game.SceneManager;
    scenes.RegisterScene("rift", std::make_unique<RiftScene>(&game));
    scenes.GetScene<BeStandardBaseScene>("rift")->Prepare();

    scenes.RequestSceneChange("rift");
    scenes.ApplyPendingSceneChange();

    return game.Run();
}
