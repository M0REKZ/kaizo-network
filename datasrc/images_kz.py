# Copyright (C) Benjamín Gajardo (also known as +KZ)

# Crown
kz_image_crown = Image("kz_crown", "kz/crown.png")
container.images.Add(kz_image_crown)
set_kz_crown = SpriteSet("kz_crown", kz_image_crown, 1, 1)
container.spritesets.Add(set_kz_crown)
container.sprites.Add(Sprite("kz_crown", set_kz_crown, 0, 0, 1, 1))

# Turrets
kz_image_turret_1 = Image("kz_turret_1", "kz/Turret_1.png")
kz_image_turret_2 = Image("kz_turret_2", "kz/Turret_2.png")
container.images.Add(kz_image_turret_1)
container.images.Add(kz_image_turret_2)
set_kz_turret_1 = SpriteSet("kz_turret_1", kz_image_turret_1, 1, 1)
set_kz_turret_2 = SpriteSet("kz_turret_2", kz_image_turret_2, 1, 1)
container.spritesets.Add(set_kz_turret_1)
container.spritesets.Add(set_kz_turret_2)
container.sprites.Add(Sprite("kz_turret_1", set_kz_turret_1, 0, 0, 1, 1))
container.sprites.Add(Sprite("kz_turret_2", set_kz_turret_2, 0, 0, 1, 1))

# Mine
kz_image_mine = Image("kz_mine", "kz/mine.png")
container.images.Add(kz_image_mine)
set_kz_mine = SpriteSet("kz_mine", kz_image_mine, 1, 1)
container.spritesets.Add(set_kz_mine)
container.sprites.Add(Sprite("kz_mine", set_kz_mine, 0, 0, 1, 1))

# InstaShield Shield
kz_image_shield = Image("kz_shield", "kz/shield.png")
container.images.Add(kz_image_shield)
set_kz_shield = SpriteSet("kz_shield", kz_image_shield, 1, 1)
container.spritesets.Add(set_kz_shield)
container.sprites.Add(Sprite("kz_shield", set_kz_shield, 0, 0, 1, 1))

# Portal gun
kz_image_portal = Image("kz_portal", "kz/portal.png")
container.images.Add(kz_image_portal)
set_kz_portal = SpriteSet("kz_portal", kz_image_portal, 1, 1)
container.spritesets.Add(set_kz_portal)
container.sprites.Add(Sprite("kz_portal", set_kz_portal, 0, 0, 7, 3))

kz_image_portal_orange = Image("kz_portal_orange", "kz/portal_orange.png")
container.images.Add(kz_image_portal_orange)
set_kz_portal_orange = SpriteSet("kz_portal_orange", kz_image_portal_orange, 1, 1)
container.spritesets.Add(set_kz_portal_orange)
container.sprites.Add(Sprite("kz_portal_orange", set_kz_portal_orange, 0, 0, 7, 3))

# Attractor beam
kz_image_attractor = Image("kz_attractor", "kz/attractor.png")
container.images.Add(kz_image_attractor)
set_kz_attractor = SpriteSet("kz_attractor", kz_image_attractor, 1, 1)
container.spritesets.Add(set_kz_attractor)
container.sprites.Add(Sprite("kz_attractor", set_kz_attractor, 0, 0, 7, 3))

# Client Icons
kz_image_kaizoicon= Image("kz_kaizoicon", "kz/kaizoicon.png")
container.images.Add(kz_image_kaizoicon)
set_kz_kaizoicon = SpriteSet("kz_kaizoicon", kz_image_kaizoicon, 1, 1)
container.spritesets.Add(set_kz_kaizoicon)
container.sprites.Add(Sprite("kz_kaizoicon", set_kz_kaizoicon, 0, 0, 1, 1))

kz_image_chillerbotuxicon= Image("kz_chillerbotuxicon", "kz/chillerbotux.png")
container.images.Add(kz_image_chillerbotuxicon)
set_kz_chillerbotuxicon = SpriteSet("kz_chillerbotuxicon", kz_image_chillerbotuxicon, 1, 1)
container.spritesets.Add(set_kz_chillerbotuxicon)
container.sprites.Add(Sprite("kz_chillerbotuxicon", set_kz_chillerbotuxicon, 0, 0, 1, 1))

kz_image_fclienticon= Image("kz_fclienticon", "kz/fclient.png")
container.images.Add(kz_image_fclienticon)
set_kz_fclienticon = SpriteSet("kz_fclienticon", kz_image_fclienticon, 1, 1)
container.spritesets.Add(set_kz_fclienticon)
container.sprites.Add(Sprite("kz_fclienticon", set_kz_fclienticon, 0, 0, 1, 1))

kz_image_gamericon= Image("kz_gamericon", "kz/gamer.png")
container.images.Add(kz_image_gamericon)
set_kz_gamericon = SpriteSet("kz_gamericon", kz_image_gamericon, 1, 1)
container.spritesets.Add(set_kz_gamericon)
container.sprites.Add(Sprite("kz_gamericon", set_kz_gamericon, 0, 0, 1, 1))

kz_image_zillywoodsicon= Image("kz_zillywoodsicon", "kz/zillywoods.png")
container.images.Add(kz_image_zillywoodsicon)
set_kz_zillywoodsicon = SpriteSet("kz_zillywoodsicon", kz_image_zillywoodsicon, 1, 1)
container.spritesets.Add(set_kz_zillywoodsicon)
container.sprites.Add(Sprite("kz_zillywoodsicon", set_kz_zillywoodsicon, 0, 0, 1, 1))

kz_image_duckicon= Image("kz_duckicon", "kz/duckicon.png")
container.images.Add(kz_image_duckicon)
set_kz_duckicon = SpriteSet("kz_duckicon", kz_image_duckicon, 1, 1)
container.spritesets.Add(set_kz_duckicon)
container.sprites.Add(Sprite("kz_duckicon", set_kz_duckicon, 0, 0, 1, 1))

kz_image_rushieicon= Image("kz_rushieicon", "kz/rushieicon.png")
container.images.Add(kz_image_rushieicon)
set_kz_rushieicon = SpriteSet("kz_rushieicon", kz_image_rushieicon, 1, 1)
container.spritesets.Add(set_kz_rushieicon)
container.sprites.Add(Sprite("kz_rushieicon", set_kz_rushieicon, 0, 0, 1, 1))
