import tensorflow as tf

b = tf.constant([1.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0], shape=[4097, 4097])
result = tf.matmul(b, b)