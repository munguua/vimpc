/*
   Vimpc
   Copyright (C) 2010 - 2011 Nathan Sweetman

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

   song.hpp - represents information of an individual track
   */

#include "song.hpp"

#include <stdio.h>

#include "buffers.hpp"
#include "buffer/directory.hpp"
#include "buffer/library.hpp"

const std::string UnknownArtist = "Unknown Artist";
const std::string UnknownAlbum  = "Unknown Album";
const std::string UnknownTitle  = "Unknown";
const std::string UnknownURI    = "Unknown";
const std::string UnknownGenre  = "Unknown";
const std::string UnknownDate   = "Unknown";

using namespace Mpc;

Song::Song() :
   reference_ (0),
   artist_    (""),
   album_     (""),
   title_     (""),
   track_     (""),
   uri_       (""),
   genre_     (""),
   date_      (""),
   duration_  (0),
   lastFormat_(""),
   formatted_ (""),
   entry_     (NULL)
{ }

Song::Song(Song const & song) :
   reference_ (0),
   artist_    (song.Artist()),
   album_     (song.Album()),
   title_     (song.Title()),
   track_     (song.Track()),
   uri_       (song.URI()),
   genre_     (song.Genre()),
   date_      (song.Date()),
   duration_  (song.Duration()),
   lastFormat_(song.lastFormat_),
   formatted_ (song.formatted_)
{
   SetDuration(duration_);
}

Song::~Song()
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);

   reference_ = 0;

   if (entry_ != NULL)
   {
      entry_->song_ = NULL;
   }
}


int32_t Song::Reference() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return reference_;
}

/* static */ void Song::IncrementReference(Song * song)
{
   std::unique_lock<std::recursive_mutex> lock(song->mutex_);
   song->reference_ += 1;

   if ((song->entry_ != NULL) && (song->reference_ == 1))
   {
      song->entry_->AddedToPlaylist();
      Main::Directory().AddedToPlaylist(song->URI());
   }
}

/* static */ void Song::DecrementReference(Song * song)
{
   std::unique_lock<std::recursive_mutex> lock(song->mutex_);
   song->reference_ -= 1;

   if ((song->entry_ != NULL) && (song->reference_ == 0))
   {
      song->entry_->RemovedFromPlaylist();
      Main::Directory().RemovedFromPlaylist(song->URI());
   }
}

/* static */ void Song::SwapThe(std::string & String)
{
   if (pcrecpp::RE("^\\s*[tT][hH][eE]\\s+").Replace("", &String))
   {
      String += ", The";
   }
}

void Song::SetArtist(const char * artist)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   lastFormat_ = "";

   if (artist != NULL)
   {
      artist_ = artist;
   }
   else
   {
      artist_ = UnknownArtist;
   }
}

std::string const & Song::Artist() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return artist_;
}

void Song::SetAlbum(const char * album)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   lastFormat_ = "";

   if (album != NULL)
   {
      album_ = album;
   }
   else
   {
      album_ = UnknownAlbum;
   }
}

std::string const & Song::Album() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return album_;
}

void Song::SetTitle(const char * title)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   lastFormat_ = "";

   if (title != NULL)
   {
      title_ = title;
   }
   else
   {
      title_ = UnknownTitle;
   }
}

std::string const & Song::Title() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return title_;
}

void Song::SetTrack(const char * track)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   lastFormat_ = "";

   if (track != NULL)
   {
      track_ = track;
   }
   else
   {
      track = "";
   }
}

std::string const & Song::Track() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return track_;
}

void Song::SetURI(const char * uri)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   lastFormat_ = "";

   if (uri != NULL)
   {
      uri_ = uri;
   }
   else
   {
      uri_ = UnknownURI;
   }
}

std::string const & Song::URI() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return uri_;
}

void Song::SetGenre(const char * genre)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   if (genre != NULL)
   {
      genre_ = genre;
   }
   else
   {
      genre_ = UnknownGenre;
   }
}

std::string const & Song::Genre() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return genre_;
}

void Song::SetDate(const char * date)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   if (date != NULL)
   {
      date_ = date;
   }
   else
   {
      date_ = UnknownDate;
   }
}

std::string const & Song::Date() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return date_;
}

void Song::SetDuration(int32_t duration)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   lastFormat_ = "";

   duration_ = duration;

   char cduration[32];
   uint32_t const minutes = static_cast<uint32_t>(duration_ / 60);
   uint32_t const seconds = (duration_ - (minutes * 60));

   snprintf(cduration, 32, "%2d:%.2d", minutes, seconds);
   durationString_ = (std::string(cduration));
}

int32_t Song::Duration() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return duration_;
}

void Song::SetEntry(LibraryEntry * entry)
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   entry_ = entry;
}

LibraryEntry * Song::Entry() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return entry_;
}

std::string const & Song::DurationString() const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   return durationString_;
}

std::string Song::FormatString(std::string fmt) const
{
   std::unique_lock<std::recursive_mutex> lock(mutex_);
   bool valid;

   if (lastFormat_ == fmt)
   {
      return formatted_;
   }

   lastFormat_ = fmt;
   std::string::const_iterator it = fmt.begin();
   valid = true;
   return ParseString(it, valid);
}

std::string Song::ParseString(std::string::const_iterator & it, bool & valid) const
{
   typedef std::string const & (Mpc::Song::*SongFunction)() const;
   static std::map<char, SongFunction> songInfo;

   if (songInfo.size() == 0)
   {
      songInfo['a'] = &Mpc::Song::Artist;
      songInfo['A'] = &Mpc::Song::Artist;
      songInfo['b'] = &Mpc::Song::Album;
      songInfo['B'] = &Mpc::Song::Album;
      songInfo['l'] = &Mpc::Song::DurationString;
      songInfo['t'] = &Mpc::Song::Title;
      songInfo['n'] = &Mpc::Song::Track;
      songInfo['f'] = &Mpc::Song::URI;
   }

   std::string result;

   do
   {
      if (*it == '\\')
      {
         *++it;
         result += *it;
         continue;
      }
      else if (*it == '{')
      {
         *++it;
         result += ParseString(it, valid);
      }
      else if (*it == '}')
      {
         if (valid) break;
         else return "";
      }
      else if (*it == '|')
      {
         valid = !valid;
      }
      else if (*it == '%')
      {
         *++it;
         if (*it == '%')
         {
            result += *it;
         }
         else if ((*it == 'a') || (*it == 'A') ||
                  (*it == 'b') || (*it == 'B') ||
                  (*it == 'l') || (*it == 't') ||
                  (*it == 'n') || (*it == 'f'))
         {
            SongFunction Function = songInfo[*it];
            std::string val = (*this.*Function)();
            if ((*it == 'B') || (*it == 'A'))
            {
               SwapThe(val);
            }

            if ((val == "") || (val.substr(0, strlen("Unknown")) == "Unknown"))
            {
               valid = false;
            }
            else
            {
               result += val;
            }
         }
         else
         {
            valid = false;
         }
      }
      else
      {
         result += *it;
      }
   } while (*++it);

   std::unique_lock<std::recursive_mutex> lock(mutex_);
   formatted_ = result;
   return result;
}

/* vim: set sw=3 ts=3: */
