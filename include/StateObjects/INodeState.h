/*
 * header file for interface INodeState
 * written by: christian moellinger <ch.moellinger@gmail.com>
 * 02/2012 Project "BambooEngine"
 */

#pragma once

#ifndef __HEADER_INODESTATE_BAMBOOENGINE
#define __HEADER_INODESTATE_BAMBOOENGINE

class INodeState
{
public:
  virtual void Action() = 0;
};

#endif
