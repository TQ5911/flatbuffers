/*
 * Copyright 2015 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fstream>
#include <iostream>
#include "monster_generated.h"  // Already includes "flatbuffers/flatbuffers.h".

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

  // 获取对根目录的访问权限:
  int size = builder.GetSize();
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


  // 保存到本地
  {
    std::string binaryData((char*)builder.GetBufferPointer(), builder.GetSize());
    std::ofstream outFile("output1111111.mon", std::ios::binary);

    // 检查文件是否成功打开
    if (!outFile.is_open()) {
      std::cout << "无法打开文件" << std::endl;
      return 1;
    }
    // 写入数据到文件
    outFile << binaryData;
    // 关闭文件
    outFile.close();
  }

  // 从本地文件中加载
  {
    std::ifstream infile;
    infile.open("output1111111.mon", std::ios::binary | std::ios::in);
    infile.seekg(0, std::ios::end);
    int length = infile.tellg();
    infile.seekg(0, std::ios::beg);
    char *binaryData = new char[length];
    infile.read(binaryData, length);
    infile.close();

    auto monster = GetMonster(binaryData);

    std::cout << "hp : " << monster->hp() << std::endl;  // '80'
    std::cout << "mana : " << monster->mana() << std::endl;  // default value of '150'
    std::string s = monster->name()->str();
    std::cout << "name : " << monster->name()->str() << std::endl;  // "MyMonster"
  }
}
