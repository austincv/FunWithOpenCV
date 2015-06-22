#!/usr/bin/env python

'''
 Invisibility?  

'''
import cv2
import numpy as np

ix,iy = 0,0
delta = 30 #adjust this value and see what happens

def select_color(event,x,y,flags,param):
  global ix,iy,clicked
  if event == cv2.EVENT_LBUTTONDBLCLK:
    ix = x
    iy = y
    clicked = True


if __name__ == '__main__':

  cap = cv2.VideoCapture(0)
  bg = None
  flip = np.zeros((1,1,3), np.uint8)
  cv2.imshow('flip',flip)
  cv2.setMouseCallback('flip',select_color)
  global clicked, color
  clicked = False
  color = None

  while(True):

    if clicked:
      color = np.array([flip.item(iy,ix,0),flip.item(iy,ix,1),flip.item(iy,ix,2)],dtype ="uint8")
      clicked = False

    # Capture frame-by-frame
    ret, frame = cap.read()
    # Flip the frame to create a mirror effect
    flip = cv2.flip(frame, 1)
    
    if bg is not None and color is not None: 
      mask = cv2.inRange(flip,color-delta,color+delta)
      flip2 = flip
      bg2 = bg
      mask2 = 255 - mask
      o1 = cv2.bitwise_and(flip, flip2, mask = mask2)
      o2 = cv2.bitwise_and(bg, bg2, mask = mask)
      o = o1+o2
      oblur = cv2.GaussianBlur(o,(3,3),0)
      cv2.imshow('flip',oblur)
    elif bg is None:
      bg = flip
      cv2.imshow('flip', flip)
    else:
      cv2.imshow('flip', flip)



    option = cv2.waitKey(1)
    if option & 0xFF == 27:
        break
    elif option & 0xFF == ord('s'):
        bg = flip
        #cv2.imshow('bg',bg)

  # When everything done, release the capture
  cap.release()
  cv2.destroyAllWindows()

