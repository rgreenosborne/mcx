/*
 * MallinCam Control
 * Copyright (C) 2012-2018 Andrew Galasso
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mcxcomm.h"

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
# include "wx/wx.h"
#endif

#include "mcx.h"
#include "crc16.h"

#include <stdlib.h>

enum { NRESP = 10 };
static msg s_buf[NRESP];
static int s_rdpos = 0;
static int s_wrpos = 0;
static int s_empty = 1;
static wxMutex s_buflock;

static void
_put(const msg *msg)
{
  wxMutexLocker l(s_buflock);
  s_buf[s_wrpos] = *msg;
  s_wrpos = (s_wrpos + 1) % NRESP;
  s_empty = 0;
}

static bool
_get(msg *msg)
{
  wxMutexLocker l(s_buflock);
  if (s_empty)
    return false;
  *msg = s_buf[s_rdpos];
  s_rdpos = (s_rdpos + 1) % NRESP;
  if (s_rdpos == s_wrpos)
    s_empty = 1;
  return true;
}

bool
mcxcomm_init()
{
    VERBOSE("%s",__FUNCTION__);
    return true;
}

bool
mcxcomm_connect(const char *filename)
{
    // todo
    return true;
}

void
mcxcomm_disconnect()
{
}

bool
mcxcomm_send_enq()
{
    // todo - simulated
    //    simulator sends ack
    msg msg;
    msg.stx = ACK;
    _put(&msg);
    return true;
}

bool
mcxcomm_send_ack()
{
    // nothing
    return true;
}

static int s_cnt = 1;
static int s_drop_ack_pct;
static int s_drop_rsp_pct;

bool
mcxcomm_send_msg(const msg& cmd)
{
    // simulator sends ack, then response
    msg rsp;

    int32_t val = rand();

    if (s_cnt > 0) {
        if (--s_cnt == 0) {
            s_drop_ack_pct = 0;
            s_drop_rsp_pct = 0;
        }
    }
    if (val < RAND_MAX / 100 * (100 - s_drop_ack_pct)) {
        rsp.stx = ACK;
        _put(&rsp);
    }
    if (val < RAND_MAX / 100 * (100 - s_drop_rsp_pct)) {
        rsp = cmd;

        rsp.cmdrsp = 0xA0; // RSP_OK

//        if (cmd.ctrl == 0x45 && cmd.data[0] == 0x1)
//            rsp.data[4] = 12; // senseUp 128x
        if (cmd.ctrl == 0x45 && cmd.data[0] == 0x1)
            rsp.data[7] = 4; // agcManLevel
        if (cmd.ctrl == 0x45 && cmd.data[0] == 0x3)
            rsp.data[4] = 2; // agcMan

  unsigned short crc = crc16(&rsp.cmdrsp, (unsigned long) &rsp.crc_hi - (unsigned long) &rsp.cmdrsp);
  rsp.crc_hi = (u8) (crc >> 8);
  rsp.crc_lo = (u8) (crc & 0xff);

        _put(&rsp);
    }
}

bool
mcxcomm_recv(msg *msg, unsigned int timeout_ms, bool *err)
{
    *err = false;
    bool ok = _get(msg);
    if (ok)
        return ok;
    wxMilliSleep(timeout_ms / 4);
    return _get(msg);
}
