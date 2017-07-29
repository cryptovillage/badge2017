import badge
import gc
import os
import u8g2
import time
import binascii
import ble
import ujson
import _thread

ble_peer_list = ble.BLEPeerList()

class LEDs:
    """Blinkinlights"""
    animation = [[[0x00,0x00,0x00],[0x00,0x00,0x00],[0xff,0x00,0x00],[0xff,0x00,0x00],[0x00,0x00,0x00],[0x00,0x00,0x00],[0x00,0x00,0x00],[0x00,0x00,0x00]]]

    def magic_byte():
      return (0xe0 | (0x0f & BadgeSettings.settings['led_brightness'])).to_bytes(1, 'big')

    # [[R,G,B], ...]
    def led_data(colors):
      return bytearray(LEDs.magic_byte() + LEDs.int_color(colors[0][2]) + LEDs.int_color(colors[0][1]) + LEDs.int_color(colors[0][0]) +
                       LEDs.magic_byte() + LEDs.int_color(colors[1][2]) + LEDs.int_color(colors[1][1]) + LEDs.int_color(colors[1][0]) +
                       LEDs.magic_byte() + LEDs.int_color(colors[2][2]) + LEDs.int_color(colors[2][1]) + LEDs.int_color(colors[2][0]) +
                       LEDs.magic_byte() + LEDs.int_color(colors[3][2]) + LEDs.int_color(colors[3][1]) + LEDs.int_color(colors[3][0]) +
                       LEDs.magic_byte() + LEDs.int_color(colors[4][2]) + LEDs.int_color(colors[4][1]) + LEDs.int_color(colors[4][0]) +
                       LEDs.magic_byte() + LEDs.int_color(colors[5][2]) + LEDs.int_color(colors[5][1]) + LEDs.int_color(colors[5][0]) +
                       LEDs.magic_byte() + LEDs.int_color(colors[6][2]) + LEDs.int_color(colors[6][1]) + LEDs.int_color(colors[6][0]) +
                       LEDs.magic_byte() + LEDs.int_color(colors[7][2]) + LEDs.int_color(colors[7][1]) + LEDs.int_color(colors[7][0]))

    def set_leds(color_data):
      badge.update_leds(LEDs.led_data(color_data))

    def int_color(color_code):
      return color_code.to_bytes(1, 'big')

    def animation_thread():
        while True:
            curAnim = LEDs.animation
            for frame in curAnim:
                LEDs.set_leds(frame)
                time.sleep(0.1)
                gc.collect()
    
    def start_animation_thread():
        _thread.start_new_thread(LEDs.animation_thread, ())

class BadgeSettings:
    settings = {
      'led_brightness': 0x0a,
      'lcd_brightness': 255,
      'volume': 5,
      'username': '',
      'badge_uniq_data': '12345678',
      'ble_broadcast': False,
      'avatar_path': '/sdcard/images/keychain.mono'
    };
    settings_path = '/sdcard/settings.json'
    
    @staticmethod
    def set_lcd_brightness(value):
        BadgeSettings.settings['lcd_brightness'] = value
        badge.set_backlight_brightness(BadgeSettings.settings['lcd_brightness'])
        
    @staticmethod
    def set_led_brightness(value):
        BadgeSettings.settings['led_brightness'] = value
        
    @staticmethod
    def set_volume(value):
        BadgeSettings.settings['volume'] = value
        badge.config_max9723(0x80 | (0x1f & BadgeSettings.settings['volume']))
        
    @staticmethod
    def set_username(value):
        BadgeSettings.settings['username'] = value
        badge.ble_set_adv_std_data(BadgeSettings.settings['username'], BadgeSettings.settings['badge_uniq_data'])
        
    @staticmethod
    def set_ble_broadcast(value):
        BadgeSettings.settings['ble_broadcast'] = value
        badge.ble_set_adv_std_data_enabled(BadgeSettings.settings['ble_broadcast'])
        
    @staticmethod
    def set_avatar_path(value):
        BadgeSettings.settings['avatar_path'] = value
        
    @staticmethod
    def save():
        try:
            file = open(BadgeSettings.settings_path, 'wt')
            file.write(ujson.dumps(BadgeSettings.settings))
            file.close()
        except OSError:
            pass
        
    @staticmethod
    def load():
        try:
            file = open(BadgeSettings.settings_path, 'rt')
            BadgeSettings.settings = ujson.loads(file.read())
            file.close()
        except OSError:
            pass

class UserInterface:
  instance = None

  def __init__(self):
    UserInterface.instance = self
    self.last_ui = MainMenu()
    self.current_ui = MainMenu()

  def start(self):
    BadgeSettings.load()
    while True:
      e = badge.get_event()
      if e.type() == 0:
        self.current_ui.scroll_left()
      elif e.type() == 1:
        self.current_ui.scroll_right()
      elif e.type() == 3:
        self.current_ui.press_enter()
      elif e.type() == 6:
        self.current_ui.press_left_pad()
      elif e.type() == 8:
        self.current_ui.press_right_pad()
      elif e.type() == 11:
        ble_peer_list.add_peer(e.data()[0:31])

  def update_current_ui(self, new_ui):
    self.last_ui = self.current_ui
    self.current_ui = new_ui

  def go_prev_ui(self):
    if self.last_ui is None:
      self.current_ui = AvatarIdleView()
    else:
      self.current_ui = self.last_ui
      self.current_ui.redraw()
    self.last_ui = None

class TextEdit:
  def __init__(self):
    self.title = ""
    self.text = ""
    self.position = len(self.text)
    self.scroll_position = 0
    self.current_char = 'a'
    self.redraw()

  def redraw(self):
    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setFontMode(True)
    u8g2.setDrawColor(1)
    u8g2.drawStr(1, 20, self.title + ":")
    u8g2.drawStr(1, 60, "Right pad to save")
    u8g2.drawLine(0, 31, 128, 31)
    render_text = self.text[self.scroll_position:]
    cur_char_x = 1+6*len(render_text)
    u8g2.drawStr(1, 30, render_text)
    u8g2.drawBox(cur_char_x-1, 20, 7, 11)
    u8g2.setDrawColor(0)
    u8g2.drawStr(cur_char_x, 30, self.current_char)
    u8g2.setDrawColor(1)
    u8g2.sendBuffer()

  def scroll_left(self):
    char_int = ord(self.current_char)
    if (char_int == 32):
      self.current_char = chr(126)
    else:
      self.current_char = chr(char_int - 1)
    self.redraw()

  def scroll_right(self):
    char_int = ord(self.current_char)
    if (char_int == 126):
      self.current_char = chr(32)
    else:
      self.current_char = chr(char_int + 1)
    self.redraw()

  def press_enter(self):
    self.text = self.text + self.current_char
    self.position += 1
    if (len(self.text) >= 20):
      self.scroll_position = len(self.text) - 20
    self.redraw()

  def press_left_pad(self):
    if (len(self.text) == 0):
      UserInterface.instance.go_prev_ui()
    else:
      self.text = self.text[:-1]
      self.position -= 1
      if (len(self.text) >= 20):
        self.scroll_position = len(self.text) - 20
      self.redraw()

  def press_right_pad(self):
    # do nothing
    pass

class ScrollTextView:
  def __init__(self):
    self.title = "Text"
    self.body = "text"
    self.lines = (len(self.body) - 1)/21 + 1
    self.scroll_position = 0
    self.redraw()

  def clear_line(self):
    self.body = self.body + (' ' * (21 - len(self.body)%21))

  def redraw(self):
    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setFontMode(True)
    u8g2.setDrawColor(1)
    u8g2.drawStr(1, 9, self.title)
    u8g2.drawLine(0, 10, 128, 10)

    idx = 0
    while (idx < 5):
      begin = (self.scroll_position + idx) * 21
      end = begin + 21
      u8g2.drawStr(1, 20 + (idx * 10), self.body[begin:end])
      idx = idx + 1

    u8g2.setDrawColor(0)
    u8g2.drawLine(127, 10, 127, 64)
    u8g2.setDrawColor(1)
    scroll_bar_start = 10 + round(self.scroll_position / float(self.lines) * 54)
    scroll_bar_end = 10 + round(min([1.0, ((self.scroll_position + 5) / float(self.lines))]) * 54)
    u8g2.drawLine(127, scroll_bar_start, 127, scroll_bar_end)

    u8g2.sendBuffer()

  def scroll_left(self):
    if (self.scroll_position > 0):
      self.scroll_position = self.scroll_position - 1
    self.redraw()

  def scroll_right(self):
    if (self.scroll_position < self.lines - 5):
      self.scroll_position = self.scroll_position + 1
    self.redraw()

  def press_enter(self):
    UserInterface.instance.go_prev_ui()

  def press_left_pad(self):
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    # do nothing
    pass

class AvatarIdleView:
    def __init__(self):
        self.redraw()
    
    def redraw(self):
        u8g2.setFont(u8g2.u8g2_font_profont12_mr)
        u8g2.setDrawColor(0)
        u8g2.drawBox(0, 0, 128, 64)
        u8g2.setFontMode(True)
        u8g2.setDrawColor(1)
        u8g2.drawStr(1, 60, BadgeSettings.settings['username'])
        try:
            f = open(BadgeSettings.settings['avatar_path'], 'rb')
            b = f.read()
            f.close()
            u8g2.drawXBM(48,0,32,32, b)
        except OSError:
            u8g2.drawStr(48, 20, "?")
        u8g2.sendBuffer()
        
    def scroll_right(self):
        UserInterface.instance.update_current_ui(MainMenu())

    def scroll_left(self):
        UserInterface.instance.update_current_ui(MainMenu())
        
    def press_enter(self):
        UserInterface.instance.update_current_ui(MainMenu())
        
    def press_left_pad(self):
        pass
        
    def press_right_pad(self):
        pass

class Menu:
  def __init__(self):
    self.menu_name = self.init_menu_name()
    self.menu_entries = self.init_menu_entries()
    self.selected = 0
    self.scroll_position = 0
    self.redraw()

  def redraw(self):
    u8g2.setPowerSave(False)
    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setFontMode(True)
    u8g2.setDrawColor(1)
    u8g2.drawStr(1, 9, self.menu_name)
    u8g2.drawLine(0, 10, 128, 10)
    
    # loop through menu entries
    max_pos = len(self.menu_entries) - 1
    idx = 0
    while (idx < 5 and ((self.scroll_position + idx) < len(self.menu_entries))):
      if (self.selected == (self.scroll_position + idx)):
        u8g2.drawBox(0, (idx * 10) + 11, 128, 10)
        u8g2.setDrawColor(0)

      u8g2.drawStr(5, 20 + (idx * 10), self.menu_entries[self.scroll_position + idx][0])
      u8g2.setDrawColor(1)
      idx = idx + 1

    u8g2.sendBuffer()

  def scroll_left(self):
    if (self.selected > 0):
      self.selected = self.selected - 1
    if (self.selected < self.scroll_position):
      self.scroll_position = self.scroll_position - 1
    self.redraw()

  def scroll_right(self):
    if (self.selected < (len(self.menu_entries) - 1)):
      self.selected = self.selected + 1
    if (self.selected > self.scroll_position + 4):
      self.scroll_position = self.scroll_position + 1
    self.redraw()

  def press_enter(self):
    UserInterface.instance.update_current_ui(self.menu_entries[self.selected][1]())

  def press_left_pad(self):
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    # do nothing
    "nothing"

class EnumPicker:
  def __init__(self):
    self.title = "title"
    self.options = [["display name", "value"],
                    ["display", "val"]]
    self.selected = 0
    self.scroll_position = 0
    self.redraw()

  def redraw(self):
    u8g2.setPowerSave(False)
    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setFontMode(True)
    u8g2.setDrawColor(1)
    u8g2.drawStr(1, 9, self.title)
    u8g2.drawLine(0, 10, 128, 10)
    
    # loop through menu entries
    max_pos = len(self.options) - 1
    idx = 0
    while (idx < 5 and ((self.scroll_position + idx) < len(self.options))):
      if (self.selected == (self.scroll_position + idx)):
        u8g2.drawBox(0, (idx * 10) + 11, 128, 10)
        u8g2.setDrawColor(0)

      u8g2.drawStr(5, 20 + (idx * 10), self.options[self.scroll_position + idx][0])
      u8g2.setDrawColor(1)
      idx = idx + 1

    u8g2.sendBuffer()

  def scroll_left(self):
    if (self.selected > 0):
      self.selected = self.selected - 1
    if (self.selected < self.scroll_position):
      self.scroll_position = self.scroll_position - 1
    self.redraw()

  def scroll_right(self):
    if (self.selected < (len(self.options) - 1)):
      self.selected = self.selected + 1
    if (self.selected > self.scroll_position + 4):
      self.scroll_position = self.scroll_position + 1
    self.redraw()

  def press_enter(self):
    pass

  def press_left_pad(self):
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    # do nothing
    pass

class BLESettingsPicker(EnumPicker):
    def __init__(self):
        self.title = "BLE"
        self.options = [["Broadcast on", True],
                        ["Broadcast off", False]]
        self.selected = 0
        self.scroll_position = 0
        self.redraw()
        
    def press_enter(self):
        BadgeSettings.set_ble_broadcast(self.options[self.selected][1])
        UserInterface.instance.go_prev_ui()
        
class Schedule:
  def __init__(self):
    self.schedule_entries = self.init_schedule_entries()
    self.selected = 0
    self.redraw()

  def redraw(self):
    u8g2.setPowerSave(False)
    #u8g2.setFont(u8g2.u8g2_font_profont10_mr)
    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setFontMode(True)
    u8g2.setDrawColor(1)
    u8g2.drawLine(0, 41, 128, 41)
    schedule_entry = self.schedule_entries[self.selected]
    time = schedule_entry[0]
    title = schedule_entry[1]
    speaker = schedule_entry[2]
    
    u8g2.drawStr(1, 10, title[0:21])
    u8g2.drawStr(1, 20, title[21:42])
    u8g2.drawStr(1, 30, title[42:63])
    u8g2.drawStr(1, 40, title[63:84])

    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.drawStr(1, 52, speaker[0:21])
    u8g2.drawStr(1, 62, speaker[21:33])
    u8g2.drawStr(94, 62, time)

    u8g2.sendBuffer()

  def scroll_left(self):
    if (self.selected > 0):
      self.selected = self.selected - 1
    self.redraw()

  def scroll_right(self):
    if (self.selected < (len(self.schedule_entries) - 1)):
      self.selected = self.selected + 1
    self.redraw()

  def press_enter(self):
    UserInterface.instance.go_prev_ui()

  def press_left_pad(self):
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    # do nothing
    "nothing"

class NumberSelectModal:
  def __init__(self):
    self.minimum = 0
    self.maximum = 10
    self.current_value = 10
    self.selected = 10
    self.redraw()

  def set_current_value(self, final_value):
    if (final_value is True):
      #set to self.current_value
      pass
    else:
      #set to self.selected
      pass

  def redraw(self):
    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.setDrawColor(1)
    u8g2.drawBox(32, 16, 64, 32)
    u8g2.setDrawColor(0)
    u8g2.drawBox(33, 17, 62, 30)
    u8g2.setFontMode(True)
    u8g2.setDrawColor(1)
    u8g2.drawStr(44, 37, str(self.selected))
    u8g2.sendBuffer()
    self.set_current_value(False)

  def scroll_left(self):
    if (self.selected > self.minimum):
      self.selected = self.selected - 1
    self.redraw()

  def scroll_right(self):
    if (self.selected < self.maximum):
      self.selected = self.selected + 1
    self.redraw()

  def press_enter(self):
    self.current_value = self.selected
    self.set_current_value(True)
    UserInterface.instance.go_prev_ui()

  def press_left_pad(self):
    self.set_current_value(True)
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    pass

class ScreenBrightnessModal(NumberSelectModal):
  def __init__(self):
    self.minimum = 0
    self.maximum = 255
    self.current_value = BadgeSettings.settings['lcd_brightness']
    self.selected = self.current_value
    self.redraw()

  def set_current_value(self, final_value):
    if (final_value is True):
      BadgeSettings.set_lcd_brightness(self.current_value)
    else:
      BadgeSettings.set_lcd_brightness(self.selected)

class VolumeModal(NumberSelectModal):
  def __init__(self):
    self.minimum = 0
    self.maximum = 31
    self.current_value = BadgeSettings.settings['volume']
    self.selected = self.current_value
    self.redraw()

  def set_current_value(self, final_value):
    if (final_value is True):
      BadgeSettings.set_volume(self.current_value)
    else:
      BadgeSettings.set_volume(self.selected)

class LEDBrightnessModal(NumberSelectModal):
  def __init__(self):
    self.minimum = 0
    self.maximum = 15
    self.current_value = BadgeSettings.settings['led_brightness']
    self.selected = self.current_value
    self.redraw()

  def set_current_value(self, final_value):
    if (final_value is True):
      BadgeSettings.set_led_brightness(self.current_value)
    else:
      BadgeSettings.set_led_brightness(self.selected)

class UsernameEdit(TextEdit):
  def __init__(self):
    self.title = "Username"
    self.text = ""
    self.position = len(self.text)
    self.scroll_position = 0
    self.current_char = 'a'
    self.redraw()

  def press_right_pad(self):
    BadgeSettings.set_username(self.text)
    UserInterface.instance.go_prev_ui()

class CreditsView(ScrollTextView):
  def __init__(self):
    self.title = "Credits"
    self.body = ("The Crypto & Privacy "
                 "  Village DC25 Badge "
                 "  by @cryptovillage  "
                 "                     "
                 "Hardware Design:     "
                 "Karl Koscher         "
                 "            @supersat"
                 "                     "
                 "Firmware:            "
                 "Karl Koscher         "
                 "            @supersat"
                 "                     "
                 "Software:            "
                 "Karl Koscher         "
                 "            @supersat"
                 "Justin Culbertson    "
                 "              @jus341"
                 "                     "
                 "Graphic Design       "
                 "Whitney Merrill      "
                 "              @wbm312"
                 "                     "
                 "Built with:          "
                 "FreeRTOS        GPL2*"
                 "esp-idf     Apache v2"
                 "u8g2             2BSD"
                 "mad              GPL2"
                 "ESP32_MP3_Decoder    "
                 "                     "
                 "More that we forgot  "
                 "in the development   "
                 "madness.             "
                 "                     "
                 "Licenses and source  "
                 "code available from  "
                 "https://github.com/  "
                 "cryptovillage/       "
                 "badge2017/           "
                 "                     "
                 "Greetz to all the    "
                 "#badgelife krew      ")

    self.lines = (len(self.body) - 1)/21 + 1
    self.scroll_position = 0
    self.redraw()
    
class RadioStream:
    streaming = False
    
    def __init__(self):
        self.redraw()
        
    def redraw(self):
        u8g2.setFont(u8g2.u8g2_font_profont12_mr)
        u8g2.setDrawColor(0)
        u8g2.drawBox(0, 0, 128, 64)
        u8g2.setDrawColor(1)
        u8g2.drawBox(50, 27, 28, 14)
        u8g2.setDrawColor(0)
        u8g2.setFontMode(True)
        if RadioStream.streaming:
            u8g2.drawStr(52, 37, "stop")
        else:
            u8g2.drawStr(52, 37, "play")
        u8g2.sendBuffer()
    
    def scroll_right(self):
        pass
    
    def scroll_left(self):
        pass
    
    def press_right_pad(self):
        pass
    
    def press_left_pad(self):
        UserInterface.instance.go_prev_ui()
    
    def press_enter(self):
        if (RadioStream.streaming):
            # stop streaming
            badge.audio_stop()
            RadioStream.streaming = False
        else:
            # start streaming
            BadgeSettings.set_volume(BadgeSettings.settings['volume'])
            badge.audio_stream_mp3_url('http://ice1.somafm.com/defcon-128-mp3')
            RadioStream.streaming = True
        self.redraw()

class FridayTalks(Schedule):
  def init_schedule_entries(self):
    return [["10:00", "Welcome", ""],
            ["10:30","Hacking on Multiparty Computation","Matt Cheung"],
            ["11:00","SHA-3 vs the world","David Wong (NCC Group)"],
            ["12:00","Alice and Bob are Slightly Less Confused","David Huerta (Freedom of the Press Foundation)"],
            ["1:00","Protecting Users' Privacy in a Location-Critical Enterprise: The Challenges of 9-1-1 Location","Trey Forgety"],
            ["2:00","Breaking TLS: A Year in Incremental Privacy Improvements","Andrew Brandt (Symantec)"],
            ["3:00","A New Political Era: Time to start wearing tin-foil hats following the 2016 elections?","Joel Wallenstrom & Robby Mook"],
            ["4:00","Security Analysis of the Telegram IM","Tomas Susanka (CTU Prague)"],
            ["4:30","Cryptanalysis in the Time of Ransomware","Mark Mager (Endgame)"],
            ["5:30","Unfairplay (NOT RECORDED)","[anonymous panel]"]]

class FridayWorkshops(Schedule):
  def init_schedule_entries(self):
    return [["11:30", "Mansion Apartment Shack House: How To Explain Crypto To Practically Anyone", "Tarah Wheeler (Psychoholics)"],
            ["12:00", "Breaking the Uber Badge Ciphers", "Kevin Hulin"],
            ["1:00", "FeatherDuster and Cryptanalib workshop", "Daniel Crowley (NCC Group)"],
            ["3:00", "NoiseSocket: Extending Noise to Make Every TCP Connection Secure", "Dmitry Dain & Alexey Ermishkin (Virgil Security, Inc.)"],
            ["5:00", "Supersingular Isogeny Diffie-Hellman", "Deirdre Connolly"]]

class SaturdayTalks(Schedule):
  def init_schedule_entries(self):
    return [["10:00", "Welcome", ""],
            ["10:30","The Surveillance Capitalism Will Continue Until Morale Improves","J0N J4RV1S"],
            ["11:30","Privacy is Not An Add-On: Designing for Privacy from the Ground Up","Alisha Kloc"],
            ["12:00","Operational Security Lessons from the Dark Web","Shea Nangle"],
            ["1:00","The Symantec/Chrome SSL debacle - how to do this better...","Jake Williams (Rendition Infosec)"],
            ["2:00","Have you seen my naked selfies? Neither has my snoopy boyfriend. Privacy within a Relationship","Lauren Rucker"],
            ["3:00","Yet another password hashing talk","Evgeny Sidorov (Yandex)"],
            ["3:30","Core Illumination: Traffic Analysis in Cyberspace","Kenneth Geers (Senior Research Scientist, Comodo)"],
            ["4:00","rustls: modern, fast, safer TLS","Joseph Birr-Pixton (Electric Imp)"],
            ["5:00","Blue Team TLS Hugs","Lee Brotherston"],
            ["5:30","Automated Testing using Crypto Differential Fuzzing (DO NOT RECORD)","Yolan Romailler (Kudelski Security)"]]

class SaturdayWorkshops(Schedule):
  def init_schedule_entries(self):
    return [["11:00", "Implementing An Elliptic Curve in Go", "George Tankersley"],
            ["12:30", "Secrets Management in the Cloud", "Evan Johnson"],
            ["2:00", "Secure Communications in Android with TLS/SSL", "Miguel Guirao"]]

class SundayTalks(Schedule):
  def init_schedule_entries(self):
    return [["10:00", "Welcome", ""],
            ["11:30","Cypherpunks History","Ryan Lackey (ResetSecurity, Inc.)"],
            ["12:00","The Key Management Facility of the Root Zone DNSSEC KSK","Punky Duero (ICANN - PTI)"],
            ["12:30","The Policy & Business Case for Privacy By Design","Zerina Curevac (Squire Patton Boggs)"],
            ["1:30","The Why and How for Secure Automatic Patch Management","Scott Arciszewski (Paragon Initiative Enterprises, LLC)"],
            ["2:00","Closing",""]]

class SundayWorkshops(Schedule):
  def init_schedule_entries(self):
    return [["11:00", "Reasoning about Consensus Algorithms", "Zaki Manian"]]

class MainMenu(Menu):
  def init_menu_name(self):
    return "MAIN MENU"

  def init_menu_entries(self):
    return [["Avatar Picker", AvatarPicker],
            ["Animation Picker", AnimationPicker],
            ["Village Schedule", VillageScheduleMenu],
            ["Decoder Ring", DecoderRing],
            ["Radio Stream", RadioStream],
            ["Nearby", NearbyMenu],
            ["System Info", SystemInfoMenu],
            ["Settings", SettingsMenu]]

class VillageScheduleMenu(Menu):
  def init_menu_name(self):
    return "Village Schedule"

  def init_menu_entries(self):
    return [["Friday Talks", FridayTalks],
            ["Friday Workshops", FridayWorkshops],
            ["Saturday Talks", SaturdayTalks],
            ["Saturday Workshops", SaturdayWorkshops],
            ["Sunday Talks", SundayTalks],
            ["Sunday Workshops", SundayWorkshops]]

class SettingsMenu(Menu):
  def init_menu_name(self):
    return "Settings"

  def init_menu_entries(self):
    return [["Username", UsernameEdit],
            ["BLE", BLESettingsPicker],
            #["WiFi", MainMenu],
            ["Volume", VolumeModal],
            ["LED Brightness", LEDBrightnessModal],
            ["Screen Brightness", ScreenBrightnessModal]]
            #["MAC Randomization", MainMenu],
            #["Check for content", MainMenu],
            #["Check for firmware update", MainMenu]]

  def press_left_pad(self):
    BadgeSettings.save()
    UserInterface.instance.go_prev_ui()

class SystemInfoMenu(Menu):
  def init_menu_name(self):
    return "System Info"

  def init_menu_entries(self):
    return [["Firmware version", FirmwareVersionView],
            ["SD card info", SDCardInfoView],
            #["SD space used/free", MainMenu],
            #["MAC address", MainMenu],
            #["Battery Status", MainMenu],
            ["Credits", CreditsView]]

class SDCardInfoView(ScrollTextView):
  def __init__(self):
    self.content_version = "Unknown"
    try:
      content_version_file = open('/sdcard/conver.txt', 'rt')
      self.content_version = content_version_file.read()
      content_version_file.close()
    except OSError:
      pass

    self.title = "SD Card"
    self.body =  "Content Version      "
    self.body += "%21s" % (self.content_version)
    
    self.lines = (len(self.body) - 1)/21 + 1
    self.scroll_position = 0
    self.redraw()

class FirmwareVersionView(ScrollTextView):
  def __init__(self):
    self.app_version = badge.get_app_version()
    self.esp_idf_version = badge.get_esp_idf_version()
    self.title = "Firmware versions"
    self.body =  "App Version          "
    self.body += "%21s" % (self.app_version)
    self.body += "ESP-IDF Version      "
    self.body += "%21s" % (self.esp_idf_version)
    
    self.lines = (len(self.body) - 1)/21 + 1
    self.scroll_position = 0
    self.redraw()
    
class BlinkyPatterns:
  patterns = [[[[0xff,0x00,0x00], [0xff,0x90,0x00], [0xff,0xff,0x00], [0x00,0xff,0x00], [0x00,0xff,0xff], [0x00,0x00,0xff], [0x90,0x00,0xc0], [0xff,0x00,0xff]],
               [[0xff,0x90,0x00], [0xff,0xff,0x00], [0x00,0xff,0x00], [0x00,0xff,0xff], [0x00,0x00,0xff], [0x90,0x00,0xc0], [0xff,0x00,0xff], [0xff,0x00,0x00]],
               [[0xff,0xff,0x00], [0x00,0xff,0x00], [0x00,0xff,0xff], [0x00,0x00,0xff], [0x90,0x00,0xc0], [0xff,0x00,0xff], [0xff,0x00,0x00], [0xff,0x90,0x00]],
               [[0x00,0xff,0x00], [0x00,0xff,0xff], [0x00,0x00,0xff], [0x90,0x00,0xc0], [0xff,0x00,0xff], [0xff,0x00,0x00], [0xff,0x90,0x00], [0xff,0xff,0x00]],
               [[0x00,0xff,0xff], [0x00,0x00,0xff], [0x90,0x00,0xc0], [0xff,0x00,0xff], [0xff,0x00,0x00], [0xff,0x90,0x00], [0xff,0xff,0x00], [0x00,0xff,0x00]],
               [[0x00,0x00,0xff], [0x90,0x00,0xc0], [0xff,0x00,0xff], [0xff,0x00,0x00], [0xff,0x90,0x00], [0xff,0xff,0x00], [0x00,0xff,0x00], [0x00,0xff,0xff]],
               [[0x90,0x00,0xc0], [0xff,0x00,0xff], [0xff,0x00,0x00], [0xff,0x90,0x00], [0xff,0xff,0x00], [0x00,0xff,0x00], [0x00,0xff,0xff], [0x00,0x00,0xff]],
               [[0xff,0x00,0xff], [0xff,0x00,0x00], [0xff,0x90,0x00], [0xff,0xff,0x00], [0x00,0xff,0x00], [0x00,0xff,0xff], [0x00,0x00,0xff], [0x90,0x00,0xc0]]], # rainbow
              
                [[[0xff,0xff,0x00], [0xff,0xff,0x00], [0xff,0x00,0x00], [0xff,0x00,0x00], [0xff,0xff,0x00], [0xff,0xff,0x00], [0xff,0xff,0x00], [0xff,0xff,0x00]]], # red eyes
                [[[0xff,0x00,0x00], [0xff,0xff,0xff], [0xff,0x00,0x00], [0xff,0xff,0xff], [0xff,0x00,0x00], [0xff,0xff,0xff], [0xff,0x00,0x00], [0xff,0xff,0xff]]], # and!xor
                [[[0x33,0xcc,0xff], [0x33,0xcc,0xff], [0x33,0xcc,0xff], [0xff,0x00,0xff], [0xff,0x00,0xff], [0xff,0x00,0xff], [0xff,0xff,0xff], [0xff,0xff,0xff]]]] # trans

  def __init__(self):
    self.redraw()

  def redraw(self):
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setDrawColor(1)
    u8g2.sendBuffer()

class DecoderRing:
  def __init__(self):
    self.selected = 0
    self.redraw()

  def redraw(self):
    try:
      f= open('/sdcard/images/rot/rot_' + str(self.selected) + '.mono', 'rb')
      b=f.read()
      f.close()
      u8g2.drawXBM(0,0,128,64,b)
      u8g2.sendBuffer()
      gc.collect()
    except OSError:
      pass

  def scroll_left(self):
    if (self.selected == 0):
      self.selected = 25
    else:
      self.selected -= 1
    self.redraw()

  def scroll_right(self):
    if (self.selected == 25):
      self.selected = 0
    else:
      self.selected += 1
    self.redraw()

  def press_enter(self):
    UserInterface.instance.go_prev_ui()

  def press_left_pad(self):
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    self.scroll_right()


class AvatarPicker:
  avatars = ['/sdcard/images/cloud.mono',
             '/sdcard/images/sunglasses.mono',
             '/sdcard/images/keychain.mono',
             '/sdcard/images/wifi.mono']

  def __init__(self):
    self.selected = 0
    self.redraw()

  def redraw(self):
    u8g2.setFont(u8g2.u8g2_font_profont12_mr)
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setFontMode(True)
    u8g2.setDrawColor(1)
    try:
        f = open(AvatarPicker.avatars[self.selected], 'rb')
        b = f.read()
        f.close()
        u8g2.drawXBM(48,0,32,32, b)
    except OSError:
        u8g2.drawStr(48, 20, "?")
    u8g2.sendBuffer()

  def scroll_left(self):
    if (self.selected > 0):
      self.selected = self.selected - 1
    self.redraw()

  def scroll_right(self):
    if (self.selected < len(AvatarPicker.avatars) - 1):
      self.selected = self.selected + 1
    self.redraw()

  def press_enter(self):
    BadgeSettings.set_avatar_path(AvatarPicker.avatars[self.selected])
    UserInterface.instance.go_prev_ui()

  def press_left_pad(self):
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    self.press_enter()
    
class AnimationPicker:
  avatars = [BlinkyPatterns.patterns[0],
             BlinkyPatterns.patterns[1],
             BlinkyPatterns.patterns[2],
             BlinkyPatterns.patterns[3]]
  my_avatar = avatars[0] # default

  def __init__(self):
    self.selected = 0
    self.redraw()

  def redraw(self):
    u8g2.setDrawColor(0)
    u8g2.drawBox(0, 0, 128, 64)
    u8g2.setDrawColor(1)
    u8g2.sendBuffer()
    LEDs.animation = AnimationPicker.avatars[self.selected]

  def scroll_left(self):
    if (self.selected > 0):
      self.selected = self.selected - 1
    self.redraw()

  def scroll_right(self):
    if (self.selected < len(AnimationPicker.avatars) - 1):
      self.selected = self.selected + 1
    self.redraw()

  def press_enter(self):
    AnimationPicker.my_avatar = AnimationPicker.avatars[self.selected]
    LEDs.animation = AnimationPicker.my_avatar
    UserInterface.instance.go_prev_ui()

  def press_left_pad(self):
    UserInterface.instance.go_prev_ui()

  def press_right_pad(self):
    self.press_enter()
    
class NearbyMenu(ScrollTextView):
  def __init__(self):
    self.title = "Nearby Badges"
    self.body = ""
    for peer in ble_peer_list.get_peers():
        peer_name = ble.get_name(peer)
        peer_type = ble.get_mfg_str(peer)
        self.body += "%-13s%8s" % (peer_name, peer_type)
    self.lines = (len(self.body) - 1)/21 + 1
    
    self.scroll_position = 0
    self.redraw()
