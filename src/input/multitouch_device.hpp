//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2013-2015 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_MULTITOUCH_DEVICE_HPP
#define HEADER_MULTITOUCH_DEVICE_HPP

#include <array>
#include <vector>

#include "input/input_device.hpp"
#include "IEventReceiver.h"

#ifdef ANDROID
#include "../../../lib/irrlicht/source/Irrlicht/CIrrDeviceAndroid.h"
#endif

#define NUMBER_OF_MULTI_TOUCHES 10

enum MultitouchButtonType
{
    BUTTON_STEERING,
    BUTTON_UP_DOWN,
    BUTTON_FIRE,
    BUTTON_NITRO,
    BUTTON_SKIDDING,
    BUTTON_LOOK_BACKWARDS,
    BUTTON_RESCUE,
    BUTTON_ESCAPE,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT
};

struct MultitouchEvent
{
    int id;
    bool touched;
    int x;
    int y;
};

struct MultitouchButton
{
    MultitouchButtonType type;
    PlayerAction action;
    bool pressed;
    unsigned int event_id;
    int x;
    int y;
    int width;
    int height;
    float axis_x;
    float axis_y;
};

class Controller;

class MultitouchDevice : public InputDevice
{
private:
    /** The list of pointers to all created buttons */
    std::vector<MultitouchButton*> m_buttons;
    
    Controller* m_controller;

    /** The parameter that is used for steering button and determines dead area
     *  in a center of button */
    float m_deadzone_center;

    /** The parameter that is used for steering button and determines dead area
     *  at the edge of button */
    float m_deadzone_edge;

    float m_orientation;
    double m_gyro_time;

	class GyroscopeFilterData
	{
		public:
		GyroscopeFilterData();
		void collectNoiseData(float data);

		// Noise filter with sane initial values, so user will be able
		// to move gyroscope during the first 10 seconds, while the noise is measured.
		// After that the values are replaced by noiseMin/noiseMax.
		float filterMin;
		float filterMax;
		float filterCenter;

		// The noise levels we're measuring.
		// Large initial values, they will decrease, but never increase.
		float noiseMin;
		float noiseMax;

		// The gyro data buffer, from which we care calculating min/max noise values.
		// The bigger it is, the more precise the calclations, and the longer it takes to converge.
		enum { NOISE_DATA_SIZE = 200, MAX_ITERATIONS = 15 };
		float noiseData[NOISE_DATA_SIZE];
		int noiseDataIdx = 0;

		// When we detect movement, we remove last few values of the measured data.
		// The movement is detected by comparing values to noiseMin/noiseMax of the previous iteration.
		int movementBackoff = 0;

		// Difference between min/max in the previous measurement iteration,
		// used to determine when we should stop measuring, when the change becomes negligilbe.
		float measuredNoiseRange = -1.0f;

		// How long the algorithm is running, to stop it when measurements are finished.
		int measurementIteration = 0;
	};

	GyroscopeFilterData m_gyro_filter;

#ifdef ANDROID
    /** Pointer to the Android irrlicht device */
    CIrrDeviceAndroid* m_android_device;
#endif

    float getSteeringFactor(float value);
    void handleControls(MultitouchButton* button);
    bool isGameRunning();

public:
    /** The array that contains data for all multitouch input events */
    std::array<MultitouchEvent, NUMBER_OF_MULTI_TOUCHES> m_events;

    MultitouchDevice();
    virtual ~MultitouchDevice();

    /** Unused function */
    bool processAndMapInput(Input::InputType type,  const int id,
                            InputManager::InputDriverMode mode,
                            PlayerAction *action, int* value = NULL)
                            {return true;}

    unsigned int getActiveTouchesCount();

    void addButton(MultitouchButtonType type, int x, int y, int width,
                   int height);
    void clearButtons();
    void reset();

    /** Returns the number of created buttons */
    unsigned int getButtonsCount() { return (unsigned int)m_buttons.size();}

    /** Returns pointer to the selected button */
    MultitouchButton* getButton(unsigned int i) {return m_buttons.at(i);}

    void activateAccelerometer();
    void deactivateAccelerometer();
    bool isAccelerometerActive();

    void activateGyroscope();
    void deactivateGyroscope();
    bool isGyroscopeActive();

    void updateAxisX(float value);
    void updateAxisY(float value);
    float getOrientation();
    void updateOrientationFromAccelerometer(float x, float y);
    void updateOrientationFromGyroscope(float z);
    void updateDeviceState(unsigned int event_id);
    void updateController();
    void updateConfigParams();

};   // MultitouchDevice

#endif
