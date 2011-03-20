#!/usr/bin/env python
import sys
import serial
import signal
import clutter
import glib

class Remote(object):
    def __init__(self, dev, baud, speed=255, turn=0.85):
        self.ser = serial.Serial(dev, baud, timeout=5)
        self.speed = speed
        self.turn = turn
        
        # Keep track of speed
        self.speed_l = 0
        self.speed_r = 0
        
        self.keys_pressed = set()
        
        # Set up Clutter stage and actors
        self.stage = clutter.Stage()
        self.stage.set_title('Nimbus Remote')
        self.stage.set_color('#000000')
        self.stage.set_size(640, 480)
        self.stage.set_user_resizable(True)
        self.stage.connect('destroy', self.quit)
        #self.stage.connect('notify::allocation', self.size_changed)
        self.stage.connect('key-press-event', self.key_pressed)
        self.stage.connect('key-release-event', self.key_released)
        
        # Allow SIGINT to pass through
        signal.signal(signal.SIGINT, signal.SIG_DFL)
        
        self.stage.show_all()
    
    def read(self):
        d = self.ser.readline()
        print d
    
    def print_state(self):
        print "SPEED:", self.speed_l, self.speed_r
    
    def set_state(self):
        self.ser.write("SPEED %d %d\n" % (self.speed_l, self.speed_r))
    
    def key_pressed(self, stage, event):
        """ Key pressed event """
        if event.keyval in self.keys_pressed:
            return
        
        self.keys_pressed.add(event.keyval)
        
        if event.keyval == clutter.keysyms.Up:
            print "Up"
            self.speed_l += self.speed
            self.speed_r += self.speed
        
        elif event.keyval == clutter.keysyms.Down:
            print "Down"
            self.speed_l -= self.speed
            self.speed_r -= self.speed
        
        elif event.keyval == clutter.keysyms.Left:
            print "Left"
            self.speed_l -= int(self.speed * self.turn)
            self.speed_r += int(self.speed * self.turn)
        
        elif event.keyval == clutter.keysyms.Right:
            print "Right"
            self.speed_l += int(self.speed * self.turn)
            self.speed_r -= int(self.speed * self.turn)
            
        elif event.keyval == clutter.keysyms.Escape:
            self.quit()
            return
        
        self.set_state()
        self.print_state()
    
    def key_released(self, stage, event):
        self.keys_pressed.remove(event.keyval)
        
        if event.keyval == clutter.keysyms.Up:
            print "Up"
            self.speed_l -= self.speed
            self.speed_r -= self.speed
        
        elif event.keyval == clutter.keysyms.Down:
            print "Down"
            self.speed_l += self.speed
            self.speed_r += self.speed
        
        elif event.keyval == clutter.keysyms.Left:
            print "Left"
            self.speed_l += int(self.speed * self.turn)
            self.speed_r -= int(self.speed * self.turn)
        
        elif event.keyval == clutter.keysyms.Right:
            print "Right"
            self.speed_l -= int(self.speed * self.turn)
            self.speed_r += int(self.speed * self.turn)
        
        self.set_state()
        self.print_state()
    
    def main(self):
        self.ser.open()
        
        clutter.main()
    
    def quit(self, *args):
        print "Exiting..."
        
        self.ser.close()
        
        clutter.main_quit()


if __name__ == '__main__':
    
    if len(sys.argv) < 2:
        print "Usage: %s <tty> [baud=9600]" % (sys.argv[0])
        sys.exit(1)
    
    dev = sys.argv[1]
    
    try:
        baud = int(sys.argv[2])
    except IndexError:
        baud = 9600

    remote = Remote(dev, baud)
    remote.main()

