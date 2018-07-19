---
layout: page
show_meta: false
title: "ATPESC 2018 Hands On Lessons"
header:
   image_fullwidth: "llnl_machine.jpg"
permalink: /lessons/
---

<table>
{% for item in site.lessons %}
<tr>
<td><a href="{{ item.url }}">{{ item.title }}</a></td>
<td>{{ item.subheadline }}</td>
</tr>
{% endfor %}
</table>
