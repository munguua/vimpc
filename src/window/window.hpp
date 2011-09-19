/*
   Vimpc
   Copyright (C) 2010 Nathan Sweetman

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   window.hpp - class representing an ncurses window
   */

#ifndef __UI__WINDOW
#define __UI__WINDOW

#include <stdint.h>
#include <string>
#include <vector>

#include "wincurses.h"

namespace Ui
{
   class Player;

   class Window
   {
   public:
      Window(int h, int w, int x, int y);
      virtual ~Window();

   private:
      Window(Window & window);
      Window & operator=(Window & window);

   public:
      virtual void Print(uint32_t line) const = 0;
      virtual void Left(Ui::Player & player, uint32_t count);
      virtual void Right(Ui::Player & player, uint32_t count);
      virtual void Confirm();
      virtual void Redraw();
      virtual void Move(int row, int column);
      virtual void Resize(int rows, int columns);

   public:
      void Erase();
      void Refresh();

   public:
      int32_t ContentSize() const { return BufferSize() - 1; }

   public:
      WINDOW * N_WINDOW() const { return window_; }

      virtual size_t BufferSize() const = 0;

   private:
      WINDOW * const window_;
   };
}

#endif
