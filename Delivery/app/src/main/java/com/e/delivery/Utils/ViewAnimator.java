package com.e.delivery.Utils;

import android.animation.Animator;
import android.animation.ValueAnimator;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.TranslateAnimation;

public class ViewAnimator {
    private static View view;
    private static int currentHeight;
    private static int newHeight;

    /**
     * @param v1 View to hide
     *
     * @param v2 View to show
    */
    public static void animatePosition(View v1, final View v2) {
        if (v1 != null) {
            TranslateAnimation ta = new TranslateAnimation(0, 0, 0, v1.getHeight());
            ta.setDuration(500);
            ta.setFillAfter(true);
            ta.setAnimationListener(new android.view.animation.Animation.AnimationListener() {
                @Override
                public void onAnimationStart(android.view.animation.Animation animation) {

                }

                @Override
                public void onAnimationEnd(android.view.animation.Animation animation) {
                    if (v2 != null) {
                        TranslateAnimation ta2 = new TranslateAnimation(0, 0, v2.getHeight(), 0);
                        ta2.setDuration(500);
                        ta2.setFillAfter(true);
                        ta2.setAnimationListener(new android.view.animation.Animation.AnimationListener() {
                            @Override
                            public void onAnimationStart(android.view.animation.Animation animation) {

                            }

                            @Override
                            public void onAnimationEnd(android.view.animation.Animation animation) {

                            }

                            @Override
                            public void onAnimationRepeat(android.view.animation.Animation animation) {

                            }
                        });
                        v2.startAnimation(ta2);
                    }
                }

                @Override
                public void onAnimationRepeat(android.view.animation.Animation animation) {

                }
            });
            v1.startAnimation(ta);
        } else if (v2 != null) {
            TranslateAnimation ta2 = new TranslateAnimation(0, 0, v2.getHeight(), 0);
            ta2.setDuration(500);
            ta2.setFillAfter(true);
            ta2.setAnimationListener(new android.view.animation.Animation.AnimationListener() {
                @Override
                public void onAnimationStart(android.view.animation.Animation animation) {

                }

                @Override
                public void onAnimationEnd(android.view.animation.Animation animation) {

                }

                @Override
                public void onAnimationRepeat(android.view.animation.Animation animation) {

                }
            });
            v2.startAnimation(ta2);
        }
    }

    public static void animateHeight(final View view, int currentHeight, final int newHeight, final  ViewAnimatorEnd vanEnd) {
        if (currentHeight == -1) {
            currentHeight = view.getMeasuredHeight();
        }
        if (currentHeight == 0) {
            currentHeight = 1;
            ViewGroup.LayoutParams layoutParams = view.getLayoutParams();
            layoutParams.height = 1;
            view.setLayoutParams(layoutParams);
            view.setVisibility(View.VISIBLE);
        }
        ValueAnimator anim = ValueAnimator.ofInt(currentHeight, newHeight);
        anim.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator valueAnimator) {
                int val = (Integer) valueAnimator.getAnimatedValue();
                ViewGroup.LayoutParams layoutParams = view.getLayoutParams();
                layoutParams.height = val;
                view.setLayoutParams(layoutParams);
            }
        });
        anim.addListener(new ValueAnimator.AnimatorListener() {

            @Override
            public void onAnimationStart(Animator animation) {

            }

            @Override
            public void onAnimationEnd(Animator animation) {
                if (vanEnd != null) {
                    vanEnd.end();
                }
                if (newHeight == 0) {
                    view.setVisibility(View.GONE);
                }
            }

            @Override
            public void onAnimationCancel(Animator animation) {

            }

            @Override
            public void onAnimationRepeat(Animator animation) {

            }
        });
        anim.setDuration(500);
        anim.start();
    }

    public interface ViewAnimatorEnd {
        void end();
    }
}
