import unittest
import numpy as np
from cv2.typing import MatLike
from utils import FrameBuffer, FrameBufferItem 

class TestFrameBuffer(unittest.TestCase):
    
    def setUp(self):
        """Create a fresh FrameBuffer instance for each test."""
        self.buffer = FrameBuffer(length=5)
    
    def test_initialization(self):
        """Test that FrameBuffer initializes correctly."""
        self.assertEqual(self.buffer.length, 5)
        self.assertEqual(self.buffer.pushIter, 0)
        self.assertEqual(self.buffer.popIter, 0)
        self.assertEqual(len(self.buffer.items), 5)
        self.assertEqual(self.buffer.overflow, False)
    
    def test_push_single_frame(self):
        """Test pushing a single frame."""
        frame = np.array([[1, 2], [3, 4]])
        self.buffer.push(seqNo=1, frame=frame)
        
        self.assertEqual(self.buffer.pushIter, 1)
        self.assertEqual(self.buffer.overflow, False)
        self.assertEqual(self.buffer.popIter, 0)
        self.assertEqual(self.buffer.items[0].seqNo, 1)
        np.testing.assert_array_equal(self.buffer.items[0].frame, frame)
    
    def test_push_multiple_frames(self):
        """Test pushing multiple frames."""
        frames = [np.array([[i, i+1]]) for i in range(3)]
        
        for idx, frame in enumerate(frames):
            self.buffer.push(seqNo=idx, frame=frame)
        
        self.assertEqual(self.buffer.pushIter, 3)
        self.assertEqual(self.buffer.overflow, False)
        self.assertEqual(self.buffer.popIter, 0)
        for idx in range(3):
            self.assertEqual(self.buffer.items[idx].seqNo, idx)
    
    def test_push_fill_buffer(self):
        """Test pushing until the buffer is full (with wrap around)."""
        for i in range(5):  # Push more than buffer length
            self.buffer.push(seqNo=i, frame=np.array([i]))
        
        self.assertEqual(self.buffer.pushIter, 0)
        self.assertEqual(self.buffer.popIter, 0)
        for idx in range(5):
            self.assertEqual(self.buffer.items[idx].seqNo, idx)

        self.assertEqual(self.buffer.pushIter, 0)
        self.assertEqual(self.buffer.overflow, True) 
        self.assertEqual(self.buffer.popIter, 0)              
        self.assertEqual(self.buffer.items[self.buffer.popIter].seqNo, 0)

    def test_push_wraparound_by_1(self):
        """Test pushing until the buffer is full (with wrap around)."""
        for i in range(6):  # Push more than buffer length
            self.buffer.push(seqNo=i, frame=np.array([i]))
        
        self.assertEqual(self.buffer.pushIter, 1)
        self.assertEqual(self.buffer.overflow, True)
        self.assertEqual(self.buffer.popIter, 1)
        self.assertEqual(self.buffer.items[self.buffer.pushIter].seqNo, 1)
        self.assertEqual(self.buffer.items[self.buffer.popIter].seqNo, 1)

    def test_push_wraparound_by_2(self):
        """Test that pushIter wraps around correctly."""
        for i in range(7):  # Push more than buffer length
            self.buffer.push(seqNo=i, frame=np.array([i]))
        
        # After 7 pushes with length 5, pushIter should be at (7 % 5) = 2
        self.assertEqual(self.buffer.pushIter, 2)
        self.assertEqual(self.buffer.overflow, True)
        self.assertEqual(self.buffer.popIter, 2)
        self.assertEqual(self.buffer.items[self.buffer.pushIter].seqNo, 2)
        self.assertEqual(self.buffer.items[self.buffer.popIter].seqNo, 2)

    def test_push_wraparound_by_5(self):
        """Test that pushIter wraps around correctly."""
        for i in range(10):  # Push more than buffer length
            self.buffer.push(seqNo=i, frame=np.array([i]))
        
        # After 10 pushes with length 5, pushIter should be at (10 % 5) = 0
        self.assertEqual(self.buffer.pushIter, 0)
        self.assertEqual(self.buffer.overflow, True)
        self.assertEqual(self.buffer.popIter, 0)
        self.assertEqual(self.buffer.items[self.buffer.pushIter].seqNo, 5)
        self.assertEqual(self.buffer.items[self.buffer.popIter].seqNo, 5)

    def test_pop_empty_buffer(self):
        """Test popping from an empty buffer returns None."""
        result = self.buffer.pop()
        self.assertIsNone(result)
    
    def test_pop_single_frame(self):
        """Test popping a single frame."""
        frame = np.array([[1, 2]])
        self.buffer.push(seqNo=1, frame=frame)
        
        result = self.buffer.pop()
        
        self.assertIsNotNone(result)
        self.assertEqual(result.seqNo, 1)
        self.assertEqual(self.buffer.overflow, False)
        np.testing.assert_array_equal(result.frame, frame)
    
    def test_pop_fifo_order(self):
        """Test that frames are popped in FIFO order."""
        for i in range(3):
            self.buffer.push(seqNo=i, frame=np.array([i]))
        
        for i in range(3):
            result = self.buffer.pop()
            self.assertEqual(result.seqNo, i)
    
    def test_pop_after_wraparound_by_2(self):
        """Test popping after buffer wraparound."""
        # Fill buffer beyond its length
        for i in range(7):
            self.buffer.push(seqNo=i, frame=np.array([i]))
        
        # Pop should still work in FIFO orde
        result = self.buffer.pop()
        #self.assertEqual(self.buffer.overflow, True)
        self.assertEqual(result.seqNo, 2)

    def test_3_pops_after_wraparound_by_2(self):
        """Test popping after buffer wraparound."""
        # Fill buffer beyond its length
        for i in range(7):
            self.buffer.push(seqNo=i, frame=np.array([i]))
        
        # Pop should still work in FIFO orde
        result = self.buffer.pop()
        self.assertEqual(self.buffer.overflow, True)
        self.assertEqual(result.seqNo, 2)

        result = self.buffer.pop()
        self.assertEqual(self.buffer.overflow, True)
        self.assertEqual(result.seqNo, 3)   

        result = self.buffer.pop()
        self.assertEqual(self.buffer.overflow, False)
        self.assertEqual(result.seqNo, 4) 

    def test_pop_when_caught_up(self):
        """Test that pop returns None when buffer is caught up."""
        frame = np.array([[1, 2]])
        self.buffer.push(seqNo=1, frame=frame)
        
        # Pop the only item
        result = self.buffer.pop()
        self.assertEqual(1, self.buffer.popIter)
        self.assertIsNotNone(result)
        
        # Pop again when caught up
        result = self.buffer.pop()
        self.assertEqual(1, self.buffer.popIter)
        self.assertIsNone(result)
    
    def test_push_pop_interleaved(self):
        """Test interleaving push and pop operations."""
        self.buffer.push(seqNo=1, frame=np.array([1]))
        self.buffer.push(seqNo=2, frame=np.array([2]))
        
        result1 = self.buffer.pop()
        self.assertEqual(result1.seqNo, 1)
        
        self.buffer.push(seqNo=3, frame=np.array([3]))
        
        result2 = self.buffer.pop()
        self.assertEqual(result2.seqNo, 2)
        
        result3 = self.buffer.pop()
        self.assertEqual(result3.seqNo, 3)
    
    def test_frame_buffer_item_creation(self):
        """Test FrameBufferItem initialization."""
        frame = np.array([[1, 2, 3]])
        item = FrameBufferItem(seqNo=5, frame=frame)
        
        self.assertEqual(item.seqNo, 5)
        np.testing.assert_array_equal(item.frame, frame)


if __name__ == '__main__':
    unittest.main()
