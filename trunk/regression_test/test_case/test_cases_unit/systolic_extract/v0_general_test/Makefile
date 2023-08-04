CASES_FIXED = cnn_fixed mm_fixed
CASES_ALL = $(CASES_FIXED) \
			cnn_access_pattern_r1_r2 cnn_access_pattern_r2_r1 \
			mm_access_pattern_i1_i2 mm_access_pattern_i2_i1 \
			cnn_outer_loop_num_miss_r0

.PHONY: clean

clean:
	for dir in $(CASES_ALL); do \
		$(MAKE) -C $$dir -f Makefile $@; \
		done
