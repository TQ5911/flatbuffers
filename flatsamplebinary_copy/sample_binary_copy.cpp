// flatsamplebinary_copy.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "monster_copy_generated.h"

using namespace MyGame::Sample;

// Example how to use FlatBuffers to create and read binary buffers.

int main(int /*argc*/, const char * /*argv*/[]) {
  // 通过算法建立一个序列化的缓冲区:
  flatbuffers::FlatBufferBuilder builder;

  // 连载一些怪物的武器:一把“剑”和一把“斧头”。
  auto weapon_one_name = builder.CreateString("Sword");
  // 定义武器伤害为3
  short weapon_one_damage = 3;

  auto weapon_two_name = builder.CreateString("Axe");
  // 定义武器伤害为5
  short weapon_two_damage = 5;

  // 使用“CreateWeapon”快捷方式创建所有字段设置的武器
  auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
  auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage);

  // 从' std::vector '创建一个FlatBuffer的' vector
  std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
  weapons_vector.push_back(sword);
  weapons_vector.push_back(axe);
  auto weapons = builder.CreateVector(weapons_vector);

  // 序列化Monster所需的其他对象
  auto position = Vec3(1.0f, 2.0f, 3.0f);

  auto name = builder.CreateString("MyMonster");

  unsigned char inv_data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  auto inventory = builder.CreateVector(inv_data, 10);

  // 设置所有字段创建怪物的快捷方式:
  auto orc = CreateMonster(builder, &position, 150, 80, name, inventory,
                           Color_Red, weapons, Equipment_Weapon, axe.Union());

  builder.Finish(orc);  // 序列化对象的根

  // 可以存储在磁盘上或通过网络发送

  // 获取builder.GetSize()字节的builder.GetBufferPointe

  // 相反，我们将立即访问它(就好像我们刚刚收到它一样)。

  // builder.GetBufferPointer()是生成二进制数据
  // GetMonster从二进制数据中生成Monster对象
  // 获取对根目录的访问权限:
  auto monster = GetMonster(builder.GetBufferPointer());

  // 从FlatBuffer获取并测试一些标量类型。
  assert(monster->hp() == 80);
  assert(monster->mana() == 150);  // default
  assert(monster->name()->str() == "MyMonster");

  // 获取并测试FlatBuffer' struct '的字段
  auto pos = monster->pos();
  assert(pos);
  assert(pos->z() == 3.0f);
  (void)pos;

  // 从“库存”FlatBuffer的“向量”中获得一个元素测试
  auto inv = monster->inventory();
  assert(inv);
  assert(inv->Get(9) == 9);
  (void)inv;

  // 获取并测试“武器”FlatBuffers的“矢量”
  std::string expected_weapon_names[] = { "Sword", "Axe" };
  short expected_weapon_damages[] = { 3, 5 };
  auto weps = monster->weapons();
  for (unsigned int i = 0; i < weps->size(); i++) {
    assert(weps->Get(i)->name()->str() == expected_weapon_names[i]);
    assert(weps->Get(i)->damage() == expected_weapon_damages[i]);
  }
  (void)expected_weapon_names;
  (void)expected_weapon_damages;

  // 获取并测试“设备”接头(“装备”字段)
  assert(monster->equipped_type() == Equipment_Weapon);
  auto equipped = static_cast<const Weapon *>(monster->equipped());
  assert(equipped->name()->str() == "Axe");
  assert(equipped->damage() == 5);
  (void)equipped;

  printf("The FlatBuffer was successfully created and verified!\n");
}
