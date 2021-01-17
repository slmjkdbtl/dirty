// wengwengweng

#ifndef D_COL_H
#define D_COL_H

bool d_pt_rect(vec2 pt, rect r);
bool d_rect_rect(rect r1, rect r2);
bool d_line_line(line2 l1, line2 l2);
bool d_line_rect(line2 l, rect r);
bool d_pt_circle(vec2 pt, circle c);
bool d_circle_circle(circle c1, circle c2);
bool d_line_circle(line2 l, circle c);

#endif

#ifdef D_IMPL
#define D_COL_IMPL
#endif

#ifdef D_COL_IMPL
#ifndef D_COL_IMPL_ONCE
#define D_COL_IMPL_ONCE

static void fix_rect(rect *r) {
	vec2 pp1 = vec2_min(r->p1, r->p2);
	vec2 pp2 = vec2_max(r->p1, r->p2);
	r->p1 = pp1;
	r->p2 = pp2;
}

bool d_pt_rect(vec2 pt, rect r) {
	fix_rect(&r);
	return
		pt.x >= r.p1.x
		&& pt.x <= r.p2.x
		&& pt.y >= r.p1.y
		&& pt.y <= r.p2.y;
}

bool d_rect_rect(rect r1, rect r2) {
	fix_rect(&r1);
	fix_rect(&r2);
	return
		r1.p2.x >= r2.p1.x
		&& r1.p1.x <= r2.p2.x
		&& r1.p2.y >= r2.p1.y
		&& r1.p1.y <= r2.p2.y;
}

bool d_line_line(line2 l1, line2 l2) {
	float a =
		(
			(l2.p2.x - l2.p1.x)
			* (l1.p1.y - l2.p1.y)
			- (l2.p2.y - l2.p1.y)
			* (l1.p1.x - l2.p1.x)
		)
		/
		(
			(l2.p2.y - l2.p1.y)
			* (l1.p2.x - l1.p1.x)
			- (l2.p2.x - l2.p1.x)
			* (l1.p2.y - l1.p1.y)
		);
	float b =
		(
			(l1.p2.x - l1.p1.x)
			* (l1.p1.y - l2.p1.y)
			- (l1.p2.y - l1.p1.y)
			* (l1.p1.x - l2.p1.x)
		)
		/
		(
			(l2.p2.y - l2.p1.y)
			* (l1.p2.x - l1.p1.x)
			- (l2.p2.x - l2.p1.x)
			* (l1.p2.y - l1.p1.y)
		);
	return a >= 0.0 && a <= 1.0 && b >= 0.0 && b <= 1.0;
}

bool d_line_rect(line2 l, rect r) {
	if (pt_rect(l.p1, r) || pt_rect(l.p2, r)) {
		return true;
	}
	return
		line_line(l, line2f(r.p1, vec2f(r.p2.x, r.p1.y)))
		|| line_line(l, line2f(vec2f(r.p2.x, r.p1.y), r.p2))
		|| line_line(l, line2f(r.p2, vec2f(r.p1.x, r.p2.y)))
		|| line_line(l, line2f(vec2f(r.p1.x, r.p2.y), r.p1));
}

bool d_pt_circle(vec2 pt, circle c) {
	return vec2_dist(pt, c.center) <= c.radius;
}

bool d_circle_circle(circle c1, circle c2) {
	return vec2_dist(c1.center, c2.center) <= c1.radius + c2.radius;
}

#endif
#endif

