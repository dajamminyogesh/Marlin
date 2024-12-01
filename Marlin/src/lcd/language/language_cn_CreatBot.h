/**
 * language_cn_CreatBot.h
 *
 *  Created on: 2023/03/14
 *      Author: CreatBot LYN
 *
 *  For DWIN_LCD of CreatBot, custom code based GB2312/GBK.
 */
#pragma once

#include "../../inc/MarlinConfigPre.h"

/**
 * Simplified Chinese
 */
namespace Language_cn_CreatBot {
  using namespace Language_en;  // 未定义的字符默认使用英语

  constexpr uint8_t CHARSIZE = 2;
  LSTR              LANGUAGE = _UxGT("Simplified Chinese");

#if ENABLED(CreatBot_DGUS_V5)
  LSTR WELCOME_MSG           = MACHINE_NAME _UxGT(" \xA1\xA1\xA1\xA2\xA1\xA3\xA1\xA4.");      // 准备就绪.
  LSTR MSG_PRINT_PAUSED      = _UxGT("\xA1\xA7\xA1\xA8\xA1\xA9\xA1\xAC\xA1\xAD.");            // 打印已暂停.
  LSTR MSG_PRINTING          = _UxGT("\xA1\xA5\xA1\xA6\xA1\xA7\xA1\xA8...");                  // 正在打印...
  LSTR MSG_PRINT_ABORTED     = _UxGT("\xA1\xA7\xA1\xA8\xA1\xA9\xA1\xAE\xA1\xAF.");            // 打印已取消.
  LSTR MSG_PRINT_DONE        = _UxGT("\xA1\xA7\xA1\xA8\xA1\xA9\xA1\xAA\xA1\xAB.");            // 打印已完成.
  LSTR MSG_HEATING           = _UxGT("\xA1\xB0\xA1\xB1\xA1\xB2\xA1\xB3\xA1\xB4...");          // 喷头预热中...
  LSTR MSG_BED_HEATING       = _UxGT("\xA1\xB5\xA1\xB6\xA1\xB7\xA1\xB2\xA1\xB3\xA1\xB4...");  // 工作台预热中...
  LSTR MSG_MEDIA_INSERTED    = _UxGT("U\xA1\xC8\xA1\xA9\xA1\xC0\xA1\xC1.");                   // U盘已插入.
  LSTR MSG_MEDIA_REMOVED     = _UxGT("U\xA1\xC8\xA1\xA9\xA1\xC2\xA1\xC3.");                   // U盘已拔出.
  LSTR MSG_MEDIA_WAITING     = _UxGT("\xA1\xC9\xA1\xCASD\xA1\xBF\xA1\xC0\xA1\xC1...");        // 等待SD卡插入...
#elif ENABLED(CreatBot_DGUS_V6)
  LSTR WELCOME_MSG           = MACHINE_NAME _UxGT(" \x81\x40\x81\x41\x81\x42\x81\x43.");      // 准备就绪.
  LSTR MSG_PRINT_PAUSED      = _UxGT("\x81\x46\x81\x47\x81\x48\x81\x4B\x81\x4C.");            // 打印已暂停.
  LSTR MSG_PRINTING          = _UxGT("\x81\x44\x81\x45\x81\x46\x81\x47...");                  // 正在打印...
  LSTR MSG_PRINT_ABORTED     = _UxGT("\x81\x46\x81\x47\x81\x48\x81\x4D\x81\x4E.");            // 打印已取消.
  LSTR MSG_PRINT_DONE        = _UxGT("\x81\x46\x81\x47\x81\x48\x81\x49\x81\x4A.");            // 打印已完成.
  LSTR MSG_HEATING           = _UxGT("\x81\x4F\x81\x50\x81\x51\x81\x52\x81\x53...");          // 喷头预热中...
  LSTR MSG_BED_HEATING       = _UxGT("\x81\x54\x81\x55\x81\x56\x81\x51\x81\x52\x81\x53...");  // 工作台预热中...
  LSTR MSG_MEDIA_INSERTED    = _UxGT("U\x81\x67\x81\x48\x81\x5F\x81\x60.");                   // U盘已插入.
  LSTR MSG_MEDIA_REMOVED     = _UxGT("U\x81\x67\x81\x48\x81\x61\x81\x62.");                   // U盘已拔出.
  LSTR MSG_MEDIA_WAITING     = _UxGT("\x81\x68\x81\x69SD\x81\x5E\x81\x5F\x81\x60...");        // 等待SD卡插入...
#endif

}  // namespace Language_cn_CreatBot

/**
 * Custom code based GB2312.
 *
 *        A1A1  A1A2  A1A3  A1A4  A1A5  A1A6  A1A7  A1A8  A1A9  A1AA  A1AB  A1AC  A1AD  A1AE  A1AF
 *         准    备    就    绪    正    在     打    印    已    完    成     暂    停    取    消
 *  A1B0  A1B1  A1B2  A1B3  A1B4  A1B5  A1B6  A1B7  A1B8  A1B9  A1BA  A1BB  A1BC  A1BD  A1BE  A1BF
 *   喷    头    预    热    中    工     作    台    自    动    关     机    耗    材    未    卡
 *  A1C0  A1C1  A1C2  A1C3  A1C4  A1C5  A1C6  A1C7  A1C8  A1C9  A1CA  A1CB  A1CC  A1CD  A1CE  A1CF
 *   插    入    拔    出    空    文     件    夹    盘    等    待     用    户    操    Ⅲ    新
 *  A1D0  A1D1  A1D2  A1D3  A1D4  A1D5  A1D6  A1D7  A1D8  A1D9  A1DA  A1DB  A1DC  A1DD  A1DE  A1DF
 *   艾    尔    德
 *  A1E0  A1E1  A1E2  A1E3  A1E4  A1E5  A1E6  A1E7  A1E8  A1E9  A1EA  A1EB  A1EC  A1ED  A1EE  A1EF
 *
 *  A1F0  A1F1  A1F2  A1F3  A1F4  A1F5  A1F6  A1F7  A1F8  A1F9  A1FA  A1FB  A1FC  A1FD  A1FE
 *
 *
 *        A2A1  A2A2  A2A3  A2A4  A2A5  A2A6  A2A7  A2A8  A2A9  A2AA  A2AB  A2AC  A2AD  A2AE  A2AF
 *
 *  A2B0  A2B1  A2B2  A2B3  A2B4  A2B5  A2B6  A2B7  A2B8  A2B9  A2BA  A2BB  A2BC  A2BD  A2BE  A2BF
 *
 *  A2C0  A2C1  A2C2  A2C3  A2C4  A2C5  A2C6  A2C7  A2C8  A2C9  A2CA  A2CB  A2CC  A2CD  A2CE  A2CF
 *
 *  A2D0  A2D1  A2D2  A2D3  A2D4  A2D5  A2D6  A2D7  A2D8  A2D9  A2DA  A2DB  A2DC  A2DD  A2DE  A2DF
 *
 *  A2E0  A2E1  A2E2  A2E3  A2E4  A2E5  A2E6  A2E7  A2E8  A2E9  A2EA  A2EB  A2EC  A2ED  A2EE  A2EF
 *
 *  A2F0  A2F1  A2F2  A2F3  A2F4  A2F5  A2F6  A2F7  A2F8  A2F9  A2FA  A2FB  A2FC  A2FD  A2FE
 *
 */

/**
 * Custom code based GBK.
 *
 *  8140  8141  8142  8143  8144  8145  8146  8147  8148  8149  814A  814B  814C  814D  814E  814F
 *   准    备    就    绪    正    在     打    印    已    完     成    暂    停    取    消    喷
 *  8150  8151  8152  8153  8154  8155  8156  8157  8158  8159  815A  815B  815C  815D  815E  815F
 *   头    预    热    中    工     作    台    自    动    关     机    耗    材    未    卡    插
 *  8160  8161  8162  8163  8164  8165  8166  8167  8168  8169  816A  816B  816C  816D  816E  816F
 *   入    拔    出    空    文     件    夹    盘    等    待     用    户    操    Ⅲ    新    艾
 *  8170  8171  8172  8173  8174  8175  8176  8177  8178  8179  817A  817B  817C  817D  817E
 *   尔    德
 *  8180  8181  8182  8183  8184  8185  8186  8187  8188  8189  818A  818B  818C  818D  818E  818F
 * 
 *  8190  8191  8192  8193  8194  8195  8196  8197  8198  8199  819A  819B  819C  819D  819E  819F
 * 
 *  81A0  81A1  81A2  81A3  81A4  81A5  81A6  81A7  81A8  81A9  81AA  81AB  81AC  81AD  81AE  81AF
 * 
 *  81B0  81B1  81B2  81B3  81B4  81B5  81B6  81B7  81B8  81B9  81BA  81BB  81BC  81BD  81BE  81BF
 * 
 *  81C0  81C1  81C2  81C3  81C4  81C5  81C6  81C7  81C8  81C9  81CA  81CB  81CC  81CD  81CE  81CF
 * 
 *  81D0  81D1  81D2  81D3  81D4  81D5  81D6  81D7  81D8  81D9  81DA  81DB  81DC  81DD  81DE  81DF
 * 
 *  81E0  81E1  81E2  81E3  81E4  81E5  81E6  81E7  81E8  81E9  81EA  81EB  81EC  81ED  81EE  81EF
 * 
 *  81F0  81F1  81F2  81F3  81F4  81F5  81F6  81F7  81F8  81F9  81FA  81FB  81FC  81FD  81FE
 *
 */
