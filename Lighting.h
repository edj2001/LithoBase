#include <NeoPixelBrightnessBus.h>
#include <NeoPixelAnimator.h>

// orange light spinning clockwise - connecting to wifi
// continuous pulsing violet - error connecting to wifi

const uint16_t PixelCount = 8; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
const uint16_t AnimCount = PixelCount / 5 * 2 + 1; // we only need enough animations for the tail and one extra

const uint16_t PixelFadeDuration = 300; // third of a second
// one second divide by the number of pixels = loop once a second
const uint16_t NextPixelMoveDuration = 1000 / PixelCount; // how fast we move through the pixels

// you loose the original color the lower the dim value used
// here due to quantization
const uint8_t c_MinBrightness = 8; 
const uint8_t c_MaxBrightness = 255;
uint8_t brightness;
uint8_t lightingColour;
uint8_t lightingColourIndex;
boolean lightingState;

boolean lightingSteady;

NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma

NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// For Esp8266, the Pin is omitted and it uses GPIO3 due to DMA hardware use.  
// There are other Esp8266 alternative methods that provide more pin options, but also have
// other side effects.
// for details see wiki linked here https://github.com/Makuna/NeoPixelBus/wiki/ESP8266-NeoMethods 

RgbColor warmWhite = RgbColor(255,244,229);
RgbColor coolWhite = RgbColor(212,235,255);
RgbColor dayLight = RgbColor(255,244,242);
RgbColor metalHalide = RgbColor(242, 252, 255);
RgbColor white = RgbColor(255, 255, 255);
RgbColor black = RgbColor(0, 0, 0);
RgbColor lightBlue = RgbColor(173,216,230);
RgbColor blue = RgbColor(0,0,255);
RgbColor orange = RgbColor(255,128,0);
RgbColor yellow = RgbColor(255,255,0);
RgbColor red = RgbColor(255,0,0);
RgbColor green = RgbColor(0,255,0);
RgbColor violet = RgbColor(153,0,153);

const int maxColours = 50; //maximum allowed number of colours in colour list.
int numColours; //number of items in colour list.
int minColour; // first used array index for colour list
int maxColour; // maximum array index for colour list.

RgbColor colourList[maxColours];



// what is stored for state is specific to the need, in this case, the colors and
// the pixel to animate;
// basically what ever you need inside the animation update function
struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
    uint16_t IndexPixel; // which pixel this animation is effecting
};

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object
MyAnimationState animationState[AnimCount];
uint16_t frontPixel = 0;  // the front of the loop
RgbColor frontColor;  // the color at the front of the loop

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

void FadeOutAnimUpdate(const AnimationParam& param)
{
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);
    // apply the color to the strip
    strip.SetPixelColor(animationState[param.index].IndexPixel, 
        colorGamma.Correct(updatedColor));
}

void LoopAnimUpdate(const AnimationParam& param)
{
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // pick the next pixel inline to start animating
        // 
        frontPixel = (frontPixel + 1) % PixelCount; // increment and wrap
        if (frontPixel == 0)
        {
            // we looped, lets pick a new front color
            //frontColor = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
            //frontColor = orange;
        }

        uint16_t indexAnim;
        // do we have an animation available to use to animate the next front pixel?
        // if you see skipping, then either you are going to fast or need to increase
        // the number of animation channels
        if (animations.NextAvailableAnimation(&indexAnim, 1))
        {
            animationState[indexAnim].StartingColor = frontColor;
            if (lightingSteady )
            {
              animationState[indexAnim].EndingColor = frontColor;
            } else
            {
              animationState[indexAnim].EndingColor = RgbColor(0, 0, 0);
              
            }
            animationState[indexAnim].IndexPixel = frontPixel;

            animations.StartAnimation(indexAnim, PixelFadeDuration, FadeOutAnimUpdate);
        }
    }
}

void setupColours()
{
    colourList[0] = red;
    colourList[1] = coolWhite;
    colourList[2] = red;
    colourList[3] = orange;
    colourList[4] = yellow;
    colourList[5] = green;
    colourList[6] = blue;
    colourList[7] = violet;
    colourList[8] = white;
    numColours = 7;
    minColour = 1;
    maxColour = numColours-1+minColour;

  
}
void setupLighting()
{

    setupColours();
    lightingColour = 0;

    strip.Begin();
    strip.SetBrightness(64);
    strip.Show();

    SetRandomSeed();
  
}

void handleLights()
{
    // this is all that is needed to keep it running
    // and avoiding using delay() is always a good thing for
    // any timing related routines
    animations.UpdateAnimations();
    strip.Show();
  
}

void setLightsConnectingToWIFI()
{
    // we use the index 0 animation to time how often we move to the next
    // pixel in the strip
    frontColor = orange;
    lightingSteady = false;
    animations.StartAnimation(0, NextPixelMoveDuration, LoopAnimUpdate);
    strip.Show();
    handleLights();
  
}

void setLightsWIFIConfigMode()
{
  lightingSteady = false;
  frontColor = lightBlue;
}

void setLightsNotConnectedToWIFI()
{
  lightingSteady = false;
  frontColor = lightBlue;
}

uint8_t mapToArrayIndex(unsigned char value)
{
  /*  the input value is in the range 0-255 when the user enters 1-100.  
   *   Map the input value back to the range 1-100.
   */
   int index;
   index = round( (value * 100.0 / 255.0)+0.5);
   if (index > maxColour )
   {
      index = (index % numColours) + minColour - 1;
   }
   if (index < minColour)
   {
      index = minColour;    
   }
   if (index > maxColour)
   {
      index = maxColour;    
   }
   return index;
}

void setColour( bool state, unsigned char value)
{
  // value is 0 to 255
  int lightingColourIndex;
  
  //frontColor = coolWhite;
  lightingColour = value;
  // map back to 1 to 100 to match what user sets
  lightingColourIndex = mapToArrayIndex(lightingColour);
  frontColor = colourList[lightingColourIndex];
}


void setLightsDisplayMode()
{
  //frontColor = coolWhite;
  lightingState = true;
  setColour(lightingState, lightingColour);
  updateFauxmoLighting();
  lightingSteady = true;
}

void setLightsOff()
{
  frontColor = black;
  lightingState = false;
  updateFauxmoLighting();
}

void setLighting( bool state, unsigned char value)
{
  // value is 0 to 255
  unsigned char maxBrightness = 64;
  unsigned char targetBrightness = min(maxBrightness,value);
  strip.SetBrightness(value);

  if (state) {
    setLightsDisplayMode();
  } else
  {
    setLightsOff();
  }
  
}
boolean getLightingState()
{
  return lightingState;
}

uint8_t getLightingBrightness()
{
  return strip.GetBrightness();  
}

uint8_t getLghtingColour()
{
  return lightingColour;
}


