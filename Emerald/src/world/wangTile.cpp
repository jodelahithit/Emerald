#include "stdafx.h"

const Vector2I WangTile::m_positionOffsets[8] = {
		{0, 1},  //North
		{-1, 1}, //NorthEase
		{-1, 0}, //East
		{-1, -1},//SouthEast
		{0, -1}, //South
		{1, -1}, //SouthWest
		{1, 0},	 //West
		{1, 1}	 //Northwest
};

void WangTile::UpdateArea(int x, int z) {
	for (int i = 0; i < 8; i++) {
		Vector2I pos = Vector2I(x, z) + m_positionOffsets[i];
		Tile* tile = GetWorld()->GetTile(pos);
		if (tile && tile->IsRock()) UpdateTile(pos, tile);
	}
}

void WangTile::UpdateTile(Vector2I& pos, Tile* tile) {
	int index = 0;
	for (int i = 0; i < 8; i++) {
		Tile* surroundingTile = GetWorld()->GetTile(pos + m_positionOffsets[i]);
		if (surroundingTile != nullptr && surroundingTile->IsRock()) index += 1 << i;
	}

	switch (index) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 24:
	case 25:
	case 26:
	case 27:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 56:
	case 57:
	case 58:
	case 59:
	case 64:
	case 65:
	case 66:
	case 67:
	case 68:
	case 69:
	case 70:
	case 72:
	case 73:
	case 74:
	case 75:
	case 76:
	case 77:
	case 78:
	case 80:
	case 81:
	case 82:
	case 83:
	case 84:
	case 85:
	case 86:
	case 88:
	case 89:
	case 90:
	case 91:
	case 96:
	case 97:
	case 98:
	case 99:
	case 100:
	case 101:
	case 102:
	case 104:
	case 105:
	case 106:
	case 107:
	case 108:
	case 109:
	case 110:
	case 128:
	case 129:
	case 130:
	case 131:
	case 132:
	case 133:
	case 134:
	case 136:
	case 137:
	case 138:
	case 139:
	case 140:
	case 141:
	case 142:
	case 144:
	case 145:
	case 146:
	case 147:
	case 148:
	case 149:
	case 150:
	case 152:
	case 153:
	case 154:
	case 155:
	case 160:
	case 161:
	case 162:
	case 163:
	case 164:
	case 165:
	case 166:
	case 168:
	case 169:
	case 170:
	case 171:
	case 172:
	case 173:
	case 174:
	case 176:
	case 177:
	case 178:
	case 179:
	case 180:
	case 181:
	case 182:
	case 184:
	case 185:
	case 186:
	case 187:
	case 192:
	case 194:
	case 196:
	case 198:
	case 200:
	case 202:
	case 204:
	case 206:
	case 208:
	case 210:
	case 212:
	case 214:
	case 216:
	case 218:
	case 224:
	case 226:
	case 228:
	case 230:
	case 232:
	case 234:
	case 236:
	case 238: GetWorld()->BreakTile(pos.x, pos.y); break;
	case 255: tile->SetFull(); break;
	case 127: tile->Set(INNER, UP); break;
	case 253: tile->Set(INNER, RIGHT); break;
	case 247: tile->Set(INNER, DOWN); break;
	case 223: tile->Set(INNER, LEFT); break;
	case 28:
	case 29:
	case 30:
	case 60:
	case 61:
	case 62:
	case 92:
	case 93:
	case 94:
	case 156:
	case 157:
	case 158:
	case 188:
	case 189:
	case 190:
	case 220:
	case 222: tile->Set(OUTER, UP); break;
	case 112:
	case 113:
	case 114:
	case 115:
	case 116:
	case 117:
	case 118:
	case 120:
	case 121:
	case 122:
	case 123:
	case 240:
	case 242:
	case 244:
	case 246:
	case 248:
	case 250: tile->Set(OUTER, RIGHT); break;
	case 193:
	case 195:
	case 197:
	case 201:
	case 203:
	case 205:
	case 209:
	case 211:
	case 213:
	case 217:
	case 219:
	case 225:
	case 227:
	case 229:
	case 233:
	case 235:
	case 237: tile->Set(OUTER, DOWN); break;
	case 7:
	case 15:
	case 23:
	case 39:
	case 47:
	case 55:
	case 71:
	case 79:
	case 87:
	case 103:
	case 111:
	case 135:
	case 143:
	case 151:
	case 167:
	case 175:
	case 183:tile->Set(OUTER, LEFT); break;
	case 124:
	case 125:
	case 126:
	case 252:
	case 254: tile->Set(SLOPE, UP); break;
	case 241:
	case 243:
	case 245:
	case 249:
	case 251: tile->Set(SLOPE, RIGHT); break;
	case 199:
	case 207:
	case 215:
	case 231:
	case 239: tile->Set(SLOPE, DOWN); break;
	case 31:
	case 63:
	case 95:
	case 159:
	case 191: tile->Set(SLOPE, LEFT); break;
	case 221: tile->Set(VALLEY, UP); break;
	case 119: tile->Set(VALLEY, RIGHT); break;
	}
}