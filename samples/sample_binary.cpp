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
  // ͨ���㷨����һ�����л��Ļ�����:
  flatbuffers::FlatBufferBuilder builder;

  // ����һЩ���������:һ�ѡ�������һ�ѡ���ͷ����
  auto weapon_one_name = builder.CreateString("Sword");
  // ���������˺�Ϊ3
  short weapon_one_damage = 3;

  auto weapon_two_name = builder.CreateString("Axe");
  // ���������˺�Ϊ5
  short weapon_two_damage = 5;

  // ʹ�á�CreateWeapon����ݷ�ʽ���������ֶ����õ�����
  auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
  auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage);

  // ��' std::vector '����һ��FlatBuffer��' vector 
  std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
  weapons_vector.push_back(sword);
  weapons_vector.push_back(axe);
  auto weapons = builder.CreateVector(weapons_vector);

  // ���л�Monster�������������
  auto position = Vec3(1.0f, 2.0f, 3.0f);

  auto name = builder.CreateString("MyMonster");

  unsigned char inv_data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  auto inventory = builder.CreateVector(inv_data, 10);

  // ���������ֶδ�������Ŀ�ݷ�ʽ:
  auto orc = CreateMonster(builder, &position, 150, 80, name, inventory,
                           Color_Red, weapons, Equipment_Weapon, axe.Union());

  builder.Finish(orc);  // ���л�����ĸ�

  // ���Դ洢�ڴ����ϻ�ͨ�����緢��

  // ��ȡbuilder.GetSize()�ֽڵ�builder.GetBufferPointe

  // �෴�����ǽ�����������(�ͺ������Ǹո��յ���һ��)��

  // ��ȡ�Ը�Ŀ¼�ķ���Ȩ��:
  int size = builder.GetSize();
  auto monster = GetMonster(builder.GetBufferPointer());

  // ��FlatBuffer��ȡ������һЩ�������͡�
  assert(monster->hp() == 80);
  assert(monster->mana() == 150);  // default
  assert(monster->name()->str() == "MyMonster");

  // ��ȡ������FlatBuffer' struct '���ֶ�
  auto pos = monster->pos();
  assert(pos);
  assert(pos->z() == 3.0f);
  (void)pos;

  // �ӡ���桱FlatBuffer�ġ��������л��һ��Ԫ�ز���
  auto inv = monster->inventory();
  assert(inv);
  assert(inv->Get(9) == 9);
  (void)inv;

  // ��ȡ�����ԡ�������FlatBuffers�ġ�ʸ����
  std::string expected_weapon_names[] = { "Sword", "Axe" };
  short expected_weapon_damages[] = { 3, 5 };
  auto weps = monster->weapons();
  for (unsigned int i = 0; i < weps->size(); i++) {
    assert(weps->Get(i)->name()->str() == expected_weapon_names[i]);
    assert(weps->Get(i)->damage() == expected_weapon_damages[i]);
  }
  (void)expected_weapon_names;
  (void)expected_weapon_damages;

  // ��ȡ�����ԡ��豸����ͷ(��װ�����ֶ�)
  assert(monster->equipped_type() == Equipment_Weapon);
  auto equipped = static_cast<const Weapon *>(monster->equipped());
  assert(equipped->name()->str() == "Axe");
  assert(equipped->damage() == 5);
  (void)equipped;

  printf("The FlatBuffer was successfully created and verified!\n");


  // ���浽����
  {
    std::string binaryData((char*)builder.GetBufferPointer(), builder.GetSize());
    std::ofstream outFile("output1111111.mon", std::ios::binary);

    // ����ļ��Ƿ�ɹ���
    if (!outFile.is_open()) {
      std::cout << "�޷����ļ�" << std::endl;
      return 1;
    }
    // д�����ݵ��ļ�
    outFile << binaryData;
    // �ر��ļ�
    outFile.close();
  }

  // �ӱ����ļ��м���
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
