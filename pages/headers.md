---
layout: page
subheadline: "Header"
title: "Style your Header!"
teaser: "These are your options to style the header of each webpage individually. <em>Feeling Responsive</em> uses <a href='http://srobbin.com/jquery-plugins/backstretch/'>Backstretch by Scott Robin</a> to expand them from left to right. The width should be 1600 pixel or higher using a ratio like 16:9 or 21:9 or 2:1."
header:
   image_fullwidth: "ecp_logo2.png"
permalink: "/headers/"
---
{% include qanda question='Why did the method fail?'
   answer='The method failed because it is an explicit 
           method and the time step was too large.' %}
<ul>
    {% for post in site.tags.header %}
    <li><a href="{{ site.url }}{{ site.baseurl }}{{ post.url }}">{{ post.title }}</a></li>
    {% endfor %}
</ul>
