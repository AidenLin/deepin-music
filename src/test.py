#!/usr/bin/env python
#-*- coding: utf-8 -*-

        
from config import config
config.load()
import utils
from player import Player

import gtk
from library import MediaDB
from widget.dialog import WinDir
from source.local import ImportFolderJob
from widget.jobs_manager import jobs_manager
import gobject
import gtk
from ui_toolkit import *


from widget.headerbar import HeaderBar
gobject.threads_init()
gtk.gdk.threads_init()


class PlayList(object):
    def __init__(self):
        MediaDB.load()
        self.songs = MediaDB.get_songs("local")
        self.index = 0
        
    def get_next_song(self):    
        self.songs = MediaDB.get_songs("local")
        self.index += 1
        if self.index > len(self.songs) - 1:
            self.index = 0
        return self.songs[self.index], False   
            
    def get_previous_song(self):
        self.songs = MediaDB.get_songs("local")
        self.index -= 1
        if self.index < 0:
            self.index = len(self.songs) - 1
        return self.songs[self.index]
        
playlists = PlayList()        

class DeepinPlayer(object):
    '''Music player for linux deepin.'''
    def __init__(self):
        self.window = Application("DMuisc")
        self.window.set_default_size(320, 400)
        self.window.add_titlebar(["min", "max", "close"], None,
                                 "  Deepin Music")

        self.window.window.connect("expose-event", self.expose_cb)
        mainbox = gtk.VBox(spacing=5)
        
        MediaDB.load()
        mainbox.pack_start(HeaderBar(), False, False)

        Player.load()
        # MediaDB.connect("added", self.reload_db)
        scrolled_window = ScrolledWindow()
        
        items = [ ListItem(str(song.get_str("title")), str(song.get_str("artist")), song.get_str("#duration")) for song in MediaDB.get_songs("local") ]
        
        self.list_view = ListView()
        self.list_view.add_titles(["歌名", "艺术家", "时间"])
        self.list_view.add_items(items)
        scrolled_window.add_child(self.list_view)


        self.window.window.change_background(app_theme.get_pixbuf("skin/main.png"))

        self.window.main_box.pack_start(mainbox, False, False)
        self.window.main_box.pack_start(scrolled_window, True, True)
        self.window.main_box.pack_start(jobs_manager, False, False)
        
        
        self.player = Player        
        self.player.set_source(playlists)
        self.window.run()
        
        
    def expose_cb(self, widget, event):    
        
        return False
    
    # def reload_db(self, db, song_type, songs):
        
        # items = [ ListItem(song.get_str("title"), song.get_str("artist"), song.get_str("#duration")) for song in song ]
        # self.list_view.set_items(items)
        
    def test_cb(self, widget):    
        # print "dd"
        MediaDB.full_erase("local")
        ImportFolderJob()
        
        
    def play_cb(self, widget):
        '''play song'''
        song_uri = utils.get_uri_from_path(widget.get_filename())
        song = MediaDB.create_song({"uri": song_uri}, "local", True)

        self.player.play_new(song)
        
if __name__ == "__main__":       
    DeepinPlayer()

    

        