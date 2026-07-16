#ifndef INC_PLAYER_H_
#define INC_PLAYER_H_

namespace game
{
// Player.h
class Player
{
public:
    // C++11/Clangの属性構文。libclangでパース可能
    [[clang::annotate("my_property")]]
    int hp = 100;

    [[clang::annotate("my_property")]]
    float speed = 5.5f;

    [[clang::annotate("my_property")]]
    bool isAlive = true;

    int secretToken = 999; // 属性がないのでエディタには出ない
};
} // namespace game
#endif //INC_PLAYER_H_
