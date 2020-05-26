#include <FastLED.h>
#define NUM_LEDS 19
#define LED_PIN 2

CRGB leds[NUM_LEDS];

enum LED_TYPE {
  TYPE_STATIC,
  TYPE_DYNAMIC,
  TYPE_EXPLOSION,
  TYPE_TORCH,
};

enum DISRUPTIVE_EVENT_STAGES {
  EVENT_ALL_BLACK,
  EVENT_STATIC_FLICKER,
  EVENT_ALL_FLASH,
  EVENT_RANDOM_HUE_EXPLOSION
};



class LedGroup
{
private:
  LED_TYPE m_type;
  uint32_t* m_indexes;
  size_t m_indexCount;
  unsigned long m_lastTick;
  unsigned long m_disruptiveEventRunning;

public: 
  LED_TYPE getType() { return m_type; }
  unsigned long getLastTick() { return m_lastTick; }
  unsigned long setLastTick() { m_lastTick = millis(); }
  LedGroup* nextGroup;
  LedGroup(uint32_t* t_indexes, size_t t_indexCount, LED_TYPE t_type) : 
        m_indexes(t_indexes),
        m_indexCount(t_indexCount),
        m_lastTick(millis()),
        m_disruptiveEventRunning(false),
        nextGroup(nullptr),
        m_type(t_type) {}

  virtual void tick() = 0;
  virtual bool specialTick() = 0;

  void setColor(CRGB color) 
  {
    uint32_t* ptr = m_indexes;
    for (int i = 0; i < m_indexCount; i++) 
    {
      leds[*ptr].setRGB(color.r, color.g, color.b);
      ptr++;
    }
  }
  void setColor(CHSV color) 
  {
    uint32_t* ptr = m_indexes;
    for (int i = 0; i < m_indexCount; i++) 
    {
      leds[*ptr].setHSV(color.hue, color.sat, color.val);
      ptr++;
    }
  }

};

class DynamicLamp : public LedGroup
{
  CRGB m_staticColor;
  const float m_ToggleLightChancePerSecond = 15; // 1/30 chance every second
  bool m_lightsOn;
public:
  DynamicLamp(uint32_t t_indexes, size_t t_indexCount, CRGB t_color) : LedGroup(t_indexes, t_indexCount, TYPE_DYNAMIC), m_staticColor(t_color), m_lightsOn(true)
  {
    setColor(m_staticColor);
  }

  void tick() 
  {
    if (millis() - getLastTick() > 1000) 
    {
      setLastTick();
      if (random(0, m_ToggleLightChancePerSecond) == 1)
      {
        toggle();
      }
    }
  }

  bool specialTick()
  {
    return false;
  }

  void toggle() 
  {
    if (m_lightsOn) 
    {
      setColor(CRGB(0, 0, 0));
    }
    else 
    {
      setColor(m_staticColor);
    }
    m_lightsOn = !m_lightsOn;
  }

};

class StaticLamp : public LedGroup 
{
private:
  CRGB m_staticColor;
public:
  StaticLamp(uint32_t t_indexes, size_t t_indexCount, CRGB t_color) : LedGroup(t_indexes, t_indexCount, TYPE_STATIC), m_staticColor(t_color)
  {
  }

  bool specialTick()
  {
    return false;
  }

  void tick()
  {
    setColor(m_staticColor);
  }
};

class ExplosionLamp : public LedGroup 
{
private:
  uint16_t explosionFadeDurationMs = 400;
  uint16_t explosionDurationMs = 50;
  bool running;
public:
  ExplosionLamp(uint32_t t_indexes, size_t t_indexCount) : LedGroup(t_indexes, t_indexCount, TYPE_EXPLOSION), running(false)
  {
  }

  void tick()
  {
  }

  // This is blocking and it sucks, but that's how it is now.
  // Ideally this shouldn't use delays.
  bool specialTick() 
  {
    uint8_t hue = random(0, 255);
    CHSV color;
    for (int i = 0; i < 255; i+=6)
    {
      color = CHSV( hue, 255, i);
      setColor(color);
      if (random(0, 400) == 5)
        specialTick();
      FastLED.show();
      delay(2);
    }


    delay(random(0, 1000));
  

    for (int i = 255; i >= 0; i--)
    {
      color = CHSV( hue, 255, i);
      if (random(0, 400) == 5)
        specialTick();
      setColor(color);
      FastLED.show();
    }
    return false;
  }
};

class House 
{
private:
    bool hasEvents;
    LedGroup* groups;
    DISRUPTIVE_EVENT_STAGES m_eventPos;
    bool m_eventRunning;
    unsigned long m_lastTick;
    uint16_t m_eventDelay;
    uint16_t m_eventCounter;
    const uint16_t m_ToggleEventChancePerSecond = 30; // 1/30 chance every second


    LedGroup* getLastGroup() 
    {
      LedGroup* current = groups;
      while(current->nextGroup != nullptr) 
      {
        current = current->nextGroup;
      }
      return current;
    }

    void insertGroup(LedGroup* t_group) 
    {
      if (groups == nullptr) 
      {
        groups = t_group;
      }
      else 
      {
        LedGroup *last = getLastGroup();
        if (last != nullptr) 
        {
          last->nextGroup = t_group;
        }
      }
    }

public: 
    House() : groups(nullptr), m_eventRunning(false), m_eventPos(EVENT_ALL_BLACK), m_eventDelay(0), m_eventCounter(0), hasEvents(false)
    {

    }


    void createGroup(LED_TYPE t_type, uint32_t* indexes, size_t length) 
    {
      LedGroup* newGroup = nullptr;
      switch(t_type) 
      {
        case TYPE_STATIC:
          newGroup = new StaticLamp(indexes, length, CRGB(255, 110, 15));
        break;
        case TYPE_DYNAMIC:
          newGroup = new DynamicLamp(indexes, length, CRGB(255, 110, 15));
        break;
        case TYPE_EXPLOSION:
          hasEvents = true;
          newGroup = new ExplosionLamp(indexes, length);
        break;
      }
      if (newGroup != nullptr)
      {
        insertGroup(newGroup);
      }
    }

    void setAllColor(CRGB color) 
    {
      LedGroup* current = groups;
      while (current != nullptr)
      {
        current->setColor(color);
        current = current->nextGroup;
      }
    }

    bool normalTick() 
    {
      LedGroup* current = groups;
      while (current != nullptr)
      {
        current->tick();
        current = current->nextGroup;
      }
      return true;
    }

    bool specialTick(LED_TYPE type) 
    {
      LedGroup* current = groups;
      while (current != nullptr)
      {
        Serial.print(current->getType());
        Serial.print(" ");
        Serial.println(type);
        if (current->getType() == type)
        {
          current->specialTick();
        }
        current = current->nextGroup;
      }

    }
    
    void eventTick()
    {
      if (millis() - m_lastTick > m_eventDelay) {
        m_lastTick = millis();

        switch(m_eventPos)
        {
          case EVENT_ALL_BLACK:
            m_eventDelay = 500;
            setAllColor(CRGB(0,0,0));
            m_eventPos = EVENT_STATIC_FLICKER;
          break;

          //This is blocking for now. Should ideally use timers, no delays.
          case EVENT_STATIC_FLICKER:
            for (int i = 0; i < random(3, 6); i++) 
            {
              setAllColor(CRGB(255, 110, 15));
              FastLED.show();
              delay(random(0, 250));
              setAllColor(CRGB(0,0,0));
              FastLED.show();
              delay(random(0, 450));
            }
            Serial.println("static flicker");
            setAllColor(CRGB(0,0,0));
            delay(2000);
            m_eventPos = EVENT_RANDOM_HUE_EXPLOSION;
          break;
          case EVENT_RANDOM_HUE_EXPLOSION:
            m_lastTick = millis();
            specialTick(TYPE_EXPLOSION);
            resetEvent();
          break;
        }
      }
    }

    void resetEvent() 
    {
      m_eventRunning = false;
      m_eventPos = EVENT_ALL_BLACK;
    }

    bool isEventRunning() 
    {
      if (!hasEvents)
        return false;

      if (!m_eventRunning)
      {
        if (millis() - m_lastTick > 1000 )
        {
          m_lastTick = millis();

          if (random(0, m_ToggleEventChancePerSecond) == 1) {
            m_eventRunning = true;
          }

        }
      }
      return m_eventRunning;
    }

    void tick() 
    {
      if (isEventRunning() == false) 
      {
        normalTick();
      }
      else 
      {
        eventTick();
      }
    }
};

#define NUM_HOUSES 4
House* houses[NUM_HOUSES];
uint32_t house1_floor[] = { 0, 2, 4, 6};
uint32_t house1_room1[] = {3};
uint32_t house1_explosion[] = {1, 5, 0, 6};

uint32_t house2_floor[] = {8, 9, 10, 11};
uint32_t house2_room1[] = {7};
uint32_t house2_room2[] = {12};

uint32_t house3_floor[] = {14, 15, 16};
uint32_t house3_room1[] = {17};
uint32_t house3_room2[] = {18};

uint32_t streetlight[] = {13};

void createGroup() {

}

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  while(!Serial);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  houses[0] = new House();
  houses[0]->createGroup(TYPE_STATIC, &house1_floor[0], (size_t)(sizeof(house1_floor) / sizeof(house1_floor[0])));
  houses[0]->createGroup(TYPE_DYNAMIC, &house1_room1[0], (size_t)(sizeof(house1_room1) / sizeof(house1_room1[0])));
  houses[0]->createGroup(TYPE_EXPLOSION, &house1_explosion[0], (size_t)(sizeof(house1_explosion) / sizeof(house1_explosion[0])));

  houses[1] = new House();
  houses[1]->createGroup(TYPE_STATIC, &house2_floor[0], (size_t)(sizeof(house2_floor) / sizeof(house2_floor[0])));
  houses[1]->createGroup(TYPE_DYNAMIC, &house2_room1[0], (size_t)(sizeof(house2_room1) / sizeof(house2_room1[0])));
  houses[1]->createGroup(TYPE_DYNAMIC, &house2_room2[0], (size_t)(sizeof(house2_room2) / sizeof(house2_room2[0])));

  houses[2] = new House();
  houses[2]->createGroup(TYPE_STATIC, &house3_floor[0], (size_t)(sizeof(house3_floor) / sizeof(house3_floor[0])));
  houses[2]->createGroup(TYPE_DYNAMIC, &house3_room1[0], (size_t)(sizeof(house3_room1) / sizeof(house3_room1[0])));
  houses[2]->createGroup(TYPE_DYNAMIC, &house3_room2[0], (size_t)(sizeof(house3_room2) / sizeof(house3_room2[0])));

  houses[3] = new House();
  houses[3]->createGroup(TYPE_STATIC, &streetlight[0], (size_t)(sizeof(streetlight) / sizeof(streetlight[0])));


}

void loop() {
  for (int i = 0; i < NUM_HOUSES; i++) 
  {
    houses[i]->tick();
  }
  FastLED.show();
  delay(30);
}
