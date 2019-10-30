#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

/**
 * @brief The LuminosusConstants namespace contains all constants used in more than one component.
 */
namespace LuminosusConstants {
    /**
     * @brief triggerThreshold is the minium value to trigger an action in a node [0...1]
     */
    static const double triggerThreshold = 1.0 / 256;

    /**
     * @brief blockUpdateFps is the rate the block logic gets updated by the engine in Hz
     */
    static const int blockUpdateFps = 50;

    static const QString defaultBackgroundName = "blueprint_grey_tiled.jpg";

    // amount and complexity of graphical effects (i.e. blur and shadows):
    enum TGraphicalEffectsLevel { MIN_EFFECTS = 1, MID_EFFECTS = 2, MAX_EFFECTS = 3 };
    #ifdef Q_OS_WIN
    static const TGraphicalEffectsLevel GRAPHICAL_EFFECTS_LEVEL = MID_EFFECTS;
    #else
    static const TGraphicalEffectsLevel GRAPHICAL_EFFECTS_LEVEL = MID_EFFECTS;
    #endif
}


#endif // CONSTANTS_H
