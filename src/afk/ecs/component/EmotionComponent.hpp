#pragma once

namespace afk {
  namespace ecs {
    namespace component {
      /**
       * Emotions component
       *
       * The concept of emotion is applicable to all evolutionary levels and applies to animals as well as to humans.
       * Emotions have an evolutionary history and have evolved various forms of expression in different species.
       * Emotions served an adaptive role in helping organisms deal with key survival issues posed by the environment.
       * Despite different forms of expression of emotions in different species, there are certain common elements,
       *    or prototype patterns, that can be identified.
       * There is a small number of basic, primary, or prototype emotions.
       * All other emotions are mixed or derivative states; that is, they occur as combinations, mixtures, or compounds
       *    of the primary emotions.
       * Primary emotions are hypothethical constructs or idealized states whose properties and characteristics can only
       *    be inferred from various kinds of evidence.
       * Primary emotions can be conceptualized in terms of pairs of polar opposites.
       * All emotions vary in their degree of similarity to one another.
       * Each emotion can exist in varying degrees of intensity or levels of arousal.
       *
       * (https://en.wikipedia.org/wiki/Robert_Plutchik)
       */
      struct EmotionComponent {
        /**
         * Ecstasy - Joy - Serenity
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float serenity = 0;
        /**
         * Admiration - Trust - Acceptance
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float acceptance = 0;
        /**
         * Terror - Fear - Apprehension
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float apprehension = 0;
        /**
         * Amazement - Surprise - Distraction
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float distraction = 0;
        /**
         * Grief - Sadness - Pensiveness
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float pensiveness = 0;
        /**
         * Loathing - Disgust - Boredom
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float boredom = 0;
        /**
         * Rage - Anger - Annoyance
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float annoyance = 0;

        /**
         * Interest - Anticipation - Vigilance
         * Value from 0 (none of this emotion) to 1 (all of this emotion)
         */
        float interest = 0;
      };
    }
  }
}
