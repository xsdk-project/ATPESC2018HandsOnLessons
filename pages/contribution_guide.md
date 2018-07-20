---
layout: page
show_meta: false
title: "Contributing Guide"
subheadline: "How to create a new lesson"
header:
   image_fullwidth: "LLNL-MFEM-simulation-software-GPU-leaderboard.jpg "
permalink: "/contributing_guide/"
---

## Easiest Way To Create a New Lesson

1. The easiest way to start creating a new lesson is to copy and then start modifying
   modify the [lesson template](/lessons/lesson_template/). Starting from the top-level
   directory, copy the lesson template to a new sub-directory in `_lessons`.
```shell
mkdir _lessons/my_new_lesson
cp _lessons/lesson_template/lesson.md _lessons/my_new_lesson
```
1. Update YAML [Front Matter](http://jekyllrb.com/docs/frontmatter/). This is the
   first few lines of the file between enclosing lines containing `---`. It looks
   something like
```yaml
---
layout: page-fullwidth
title: "Lesson Template"
subheadline: "My New Lesson"
teaser: "Set your teaser here..."
permalink: "lessons/my_new_lesson/"
use_math: true
lesson: true
header:
 image_fullwidth: "2012.jpg"
---
```
   * Keep the `layout: page-fullwidth` line unchanged.
   * Define the lesson `title:` string (required)
   * If desired, define the `subheadline:` (optional)
   * If desired, define the `teaser:` (optional)
   * Define the lesson `permalink:` (required)
     * If you copied the lesson as above naming the sub-directory `my_new_lesson`
       the permalink would be `/lessons/my_new_lesson`. However, choose a directory
       name that helps to indicate the lesson topic.
   * If you intend to have [LaTeX](https://www.latex-project.org) equations
     include the `use_math: true` line. Otherwise, leave it out.
   * Keep the `lesson: true` line unchanged.
   * There are header options you can choose from. You can see a list of header
     options and examples [here](/headers/). Each option has different front-matter
     code to implement it. 
   * Select a header image in the `images` sub-folder you would like to use or
     add a new image to the `images` sub-folder and then specify the selected
     image with the `header:` tag
1. Once you have completed adjustments to the YAML frontmatter, above, you are
   ready to start writing ordinary [Markdown](https://guides.github.com/features/mastering-markdown/)
   to create your lesson.
1. You can simply replace various of the headers, text and images in the copied lesson
   with whatever you need. As you add pictures, be sure to add them to the same
   directory as the `lesson.md` file you are creating. Examine the lesson template
   to see how to link to them from your `lesson.md` file.
1. Edit `_data/navigation.yml` and add a line for your lesson in the `ATPESC 2018 Lessons`
   section of the YAML data for your lesson. If you named things as above...
```
- title: "My New Lesson"
  url: "/lessons/my_new_lesson/"
```
1. Finally, remember that in order to see the raw markup used to create any rendered
   web page, equation, etc. you can simply go to the GitHub repo for the site, and
   find the associated page there. For example, to view the raw markup for the
   [Hand Coded Heat Lesson](/lessons/hand_coded_heat/), you would go
   [here](https://github.com/xsdk-project/ATPESC2018HandsOnLessons/blob/gh-pages/_lessons/hand_coded_heat/lesson.md)
   and then hit the `Raw` button to view the
   [raw file contents](https://raw.githubusercontent.com/xsdk-project/ATPESC2018HandsOnLessons/gh-pages/_lessons/hand_coded_heat/lesson.md).
   To go to the top of the GitHub repo for this site, go [here](https://github.com/xsdk-project/ATPESC2018HandsOnLessons)

## More About GitHub Pages and Jekyll

[Jekyll](https://jekyllrb.com) allows site content developers to build beautiful
mostly static sites by composing ASCII files involving a combination of three
technologies...

* [Markdown](https://guides.github.com/features/mastering-markdown/)
(or [Textile](https://www.promptworks.com/textile)),
* YAML [Front Matter](http://jekyllrb.com/docs/frontmatter/) page configuration code
* [Liquid](https://shopify.github.io/liquid/) content filtering and page construction code

The Jekyll engine reads source `.md` files and, optionally, a number of
other CSS, HTML (and other Web technology code snippets and accouterments) stored in
supporting files and directories in the repository (when necessary) and builds the
site HTML files.

On a GitHub Pages site, this process happens automatically upon each new commit of
files to the site's repository. GitHub uses Jekyll to re-generate the site and the
changes go live shortly thereafter. See below about how to preview changes to the
site before committing them.

Within the repository, a single source `.md` file will contain not only Markdown
content, but may also contain, optionally, a YAML code block at the beginning of
the file (called its _front matter_) which holds information on how the page is
to be configured when the site is built and, optionally, Liquid instructions
which can appear anywhere in the file and which program the Jekyll engine on how
to filter, merge and combine content snippets into a generated page.

So far, we are not using either YAML Front Matter or Liquid here to build our
site. We may eventually decide we might need to do that and it would be fine
but there isn't anything yet that has indicated we have a need for that. I
mention this because we can easily get much more sophisticated than we are
currently in managing and hosting this content.

## Previewing your changes

Previewing your changes locally on your own machine requires that you have all
the necessary software. New OSX systems tend to have everything needed or it
can be obtained from HomeBrew. However, it is not essential set up your system
to preview locally. You can simply rely upon GitHub re-generating your pages
with each commit (which takes just several seconds modulo latencies in interacting
with GitHub) and then re-freshing your browser to see your changes. That does
mean each change goes _live_ once it is committed but is otherwise harmless.

### If you wanna preview locally...

If you are new to GitHub and Jekyll themed GitHub pages, read this section to
learn how to preview your work locally before committing it to GitHub. On the
other hand, if its easier for you, you can just commit changes to GitHub, see
how they turn out there and modify if you are not satisfied. That workflow
just takes a bit more time because GitHub/Jekyll backend may be delayed a
minute or two in re-generating the site. And, it also means your changes
are always going live.

This site is a GitHub pages site using a GitHub [supported](https://pages.github.com/themes/)
and the [Feeling Responsive Jekyll Theme](https://phlow.github.io/feeling-responsive/)
This means you compose content in GitHub Markdown and when you commit
changes to GitHub, Jekyll generates the HTML pages for the site automatically. But, it also
means its a bit harder for you to preview, locally, your changes before committing.

```
$ gem install github-pages
$ bundle exec jekyll serve --config _config.yml,_config_dev.yml
```

To get all the tools on my Mac (gem, bundle, jekyll), I used Homebrew.

Detailed instructions on setting up GitHub pages locally can be found [here](/how_to_preview_locally/).

## Including Math Equations

We use MathJax to include LaTex directly in markdown (`.md`) files. You can examine
the [Hand Coded Heat](/lessons/hand_coded_heat/) for examples of this.

I think it would be best put all content related to each hands-on lesson we develop here
into its own separate directory. That means all images, example codes, markdown pages, etc.
Then, we can have a separate page (or page hierarchy) that indexes the examples.

I know Jekyll has a built-in concept of a `posts` object. That is because Jekyll is designed
around the notion of supporting blogging. It may make sense to handle each hands-on kinda
sorta like a `post` in Jekyll. But, I think that also means that content related to each
lesson gets scattered across multiple directories (at least given the **default**) way that
Jekyll seems to handle `posts`. We need to investigate proper use of Jekyll further **after**
we've completed ATPESC.

## GitHub Style Primer

This section is just a copy of boilerplate content from GitHub Pages template
about how to use Markdown, etc. I have kept it here for convenience.

You can use the [editor on GitHub](https://github.com/xsdk-project/HandsOnLessons/edit/master/README.md) to maintain and preview the content for your website in Markdown files.

Whenever you commit to this repository, GitHub Pages will run [Jekyll](https://jekyllrb.com/) to rebuild the pages in your site, from the content in your Markdown files.

### Markdown

Markdown is a lightweight and easy-to-use syntax for styling your writing. It includes conventions for

```markdown
Syntax highlighted code block

# Header 1
## Header 2
### Header 3

- Bulleted
- List

1. Numbered
2. List

**Bold** and _Italic_ and `Code` text

[Link](url) and ![Image](src)
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/xsdk-project/HandsOnLessons/settings). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://help.github.com/categories/github-pages-basics/) or [contact support](https://github.com/contact) and we’ll help you sort it out.
